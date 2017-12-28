/*
 * Copyright 2017 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Voice HAL for ESP32
 *
 * Author:
 *       Andrés Calderón <andres.calderon@admobilize.com>
 *
 * MATRIX Voice ESP32 HAL is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <string>
#include <valarray>

#include "./microphone_array.h"
#include "./microphone_array_location.h"
#include "./voice_memory_map.h"

namespace matrix_hal {

MicrophoneArray::MicrophoneArray()
    : gain_(0), pdm_ratio_(0), sampling_frequency_(0), decimation_ratio_(0) {
  raw_data_.resize(kMicarrayBufferSize);

  delayed_data_.resize(kMicarrayBufferSize);

  fifos_.resize(kMicrophoneChannels);

  beamformed_.resize(NumberOfSamples());

  CalculateDelays(0.0, 0.0);
}

MicrophoneArray::~MicrophoneArray() {}

void MicrophoneArray::Setup(WishboneBus *wishbone) {
  MatrixDriver::Setup(wishbone);

  ReadConfValues();
}

//  Read audio from the FPGA and calculate beam using delay & sum method
bool MicrophoneArray::Read() {
  if (!wishbone_) return false;

  if (0) {
    if (!wishbone_->SpiReadBurst(
            kMicrophoneArrayBaseAddress,
            reinterpret_cast<unsigned char *>(&raw_data_[0]),
            sizeof(int16_t) * kMicarrayBufferSize)) {
      return false;
    }

    for (uint32_t s = 0; s < NumberOfSamples(); s++) {
      int sum = 0;
      for (uint16_t c = 0; c < kMicrophoneChannels; c++) {
        // delaying data for beamforming 'delay & sum' algorithm
        delayed_data_[s * kMicrophoneChannels + c] =
            fifos_[c].PushPop(raw_data_[s * kMicrophoneChannels + c]);

        // accumulation data for beamforming 'delay & sum' algorithm
        sum += delayed_data_[s * kMicrophoneChannels + c];
      }

      beamformed_[s] = std::min(INT16_MAX, std::max(sum, INT16_MIN));
    }
  }

  return true;
}

// Setting fifos for the 'delay & sum' algorithm
void MicrophoneArray::CalculateDelays(float azimutal_angle, float polar_angle,
                                      float radial_distance_mm,
                                      float sound_speed_mmseg) {
  //  sound source position
  float x, y, z;
  x = radial_distance_mm * std::sin(azimutal_angle) * std::cos(polar_angle);
  y = radial_distance_mm * std::sin(azimutal_angle) * std::sin(polar_angle);
  z = radial_distance_mm * std::cos(azimutal_angle);

  std::map<float, int> distance_map;

  // sorted distances from source position to each microphone
  for (int c = 0; c < kMicrophoneChannels; c++) {
    const float distance = std::sqrt(
        std::pow(micarray_location[c][0] - x, 2.0) +
        std::pow(micarray_location[c][1] - y, 2.0) + std::pow(z, 2.0));
    distance_map[distance] = c;
  }

  // fifo resize for delay compensation
  float min_distance = distance_map.begin()->first;
  for (std::map<float, int>::iterator it = distance_map.begin();
       it != distance_map.end(); ++it) {
    int delay = std::round((it->first - min_distance) * sampling_frequency_ /
                           sound_speed_mmseg);
    fifos_[it->second].Resize(delay);
  }
}

bool MicrophoneArray::GetPDMRatio() {
  if (!wishbone_) return false;
  uint16_t value;
  wishbone_->RegRead16(kMicrophoneArrayBaseAddress + 3, &value);
  pdm_ratio_ = value;
  return true;
}

bool MicrophoneArray::SetPDMRatio(uint16_t pdm_ratio) {
  if (!wishbone_) return false;
  wishbone_->RegWrite16(kMicrophoneArrayBaseAddress + 3, pdm_ratio);
  pdm_ratio_ = pdm_ratio;
  return true;
}

bool MicrophoneArray::GetDecimationRatio() {
  if (!wishbone_) return false;
  uint16_t value;
  wishbone_->RegRead16(kMicrophoneArrayBaseAddress + 1, &value);
  decimation_ratio_ = value;
  return true;
}

bool MicrophoneArray::SetDecimationRatio(uint16_t decimation_ratio) {
  if (!wishbone_) return false;
  wishbone_->RegWrite16(kMicrophoneArrayBaseAddress + 1, decimation_ratio);
  decimation_ratio_ = decimation_ratio;
  return true;
}

bool MicrophoneArray::GetGain() {
  if (!wishbone_) return false;
  uint16_t value;
  wishbone_->RegRead16(kMicrophoneArrayBaseAddress + 2, &value);
  gain_ = value;
  return true;
}

bool MicrophoneArray::SetGain(uint16_t gain) {
  if (!wishbone_) return false;
  wishbone_->RegWrite16(kMicrophoneArrayBaseAddress + 2, gain);
  gain_ = gain;
  return true;
}

bool MicrophoneArray::SetSamplingRate(uint32_t sampling_frequency) {
  sampling_frequency_ = sampling_frequency;
  uint32_t systemClock = wishbone_->FPGAFrequency();
  pdm_ratio_ = std::floor(systemClock / kPDMFrequency) - 1;
  decimation_ratio_ =
      std::floor((systemClock) / (sampling_frequency * (pdm_ratio_ + 1))) - 1;
  uint16_t maxCICBits =
      std::floor(kCICStages * (std::log(decimation_ratio_) / std::log(2)));
  gain_ = kCICWidth - maxCICBits + 1;

  SetPDMRatio(pdm_ratio_);
  SetDecimationRatio(decimation_ratio_);
  SetGain(gain_);

  return true;
}

void MicrophoneArray::ReadConfValues() {
  GetPDMRatio();
  GetDecimationRatio();
  GetGain();
  uint32_t systemClock = wishbone_->FPGAFrequency();
  sampling_frequency_ =
      std::floor((systemClock) /
                 ((pdm_ratio_ + 1) * (decimation_ratio_ + 1) * 1000)) *
      1000;
}

};  // namespace matrix_hal
