#include "physically_based_camera.h"

namespace game {

void PhysicallyBasedCamera::Update(const GLfloat l_new,
                                   const double delta_time) {
  l_avg_ = l_avg_ + (l_new - l_avg_) * (1 - std::expf(-1 * delta_time * 1.0));

  GLfloat target_ev = ComputeTargetEv(l_avg_);
  target_ev = target_ev - ev_comp_;

  aperture_ = 4.0f;
  shutter_speed_ = 1.0f / 100.0f;

  iso_ = std::clamp(ComputeIso(aperture_, shutter_speed_, target_ev), kMinIso,
                    kMaxIso);

  GLfloat evDiff = target_ev - ComputeEv(aperture_, shutter_speed_, iso_);
  aperture_ = std::clamp(aperture_ * std::powf(std::sqrt(2.0f), evDiff * 0.5f),
                         kMinAperture, kMaxAperture);

  evDiff = target_ev - ComputeEv(aperture_, shutter_speed_, iso_);
  shutter_speed_ = std::clamp(shutter_speed_ * std::powf(2.0f, -evDiff),
                              kMinShutterSpeed, kMaxShutterSpeed);

  const GLfloat l_max =
      (7800.0f / 65.0f) * (aperture_ * aperture_) / (iso_ * shutter_speed_);
  exposure_ = 1.0f / l_max;
}

const GLfloat PhysicallyBasedCamera::GetIso() const { return iso_; }

const GLfloat PhysicallyBasedCamera::GetAperture() const { return aperture_; }

const GLfloat PhysicallyBasedCamera::GetShutterSpeed() const {
  return shutter_speed_;
}

const GLfloat PhysicallyBasedCamera::GetEvComop() const { return ev_comp_; }

void PhysicallyBasedCamera::SetEvComp(const GLfloat ev_comp) {
  ev_comp_ = ev_comp;
}

const GLfloat PhysicallyBasedCamera::GetExposure() const { return exposure_; }

PhysicallyBasedCamera::PhysicallyBasedCamera(const GLfloat l_avg,
                                             const GLfloat ev_comp)
    : l_avg_(l_avg), ev_comp_(ev_comp) {
  Update(l_avg, 0.0);
}

const GLfloat PhysicallyBasedCamera::ComputeTargetEv(
    const GLfloat average_luminance) {
  return std::log2(average_luminance * 100.0f / 12.5f);
}

const GLfloat PhysicallyBasedCamera::ComputeIso(const GLfloat aperture,
                                                const GLfloat shutter_speed,
                                                const GLfloat ev) {
  return (aperture * aperture * 100.0f) / (shutter_speed * std::powf(2.0f, ev));
}

const GLfloat PhysicallyBasedCamera::ComputeEv(const GLfloat aperture,
                                               const GLfloat shutter_speed,
                                               const GLfloat iso) {
  return std::log2((aperture * aperture * 100.0f) / (shutter_speed * iso));
}

}  // namespace game