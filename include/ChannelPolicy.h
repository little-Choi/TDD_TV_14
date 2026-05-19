#ifndef CHANNEL_POLICY_H
#define CHANNEL_POLICY_H

namespace tv {

inline constexpr int kMinChannel = 0;
inline constexpr int kMaxChannel = 99;
inline constexpr int kChannelCount = kMaxChannel - kMinChannel + 1;
inline constexpr int kMaxDigitLength = 2;

} // namespace tv

#endif // CHANNEL_POLICY_H
