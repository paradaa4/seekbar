#include "Spacer.hpp"

Spacer::Spacer(const std::optional<float> maxSize, Orientation orientation)
{
    if (orientation == Orientation::Horizontal) {
        if (maxSize) {
            setSize({maxSize.value(), 0});
        } else {
            setFillWidth(true);
        }
    } else {
        if (maxSize) {
            setSize({0, maxSize.value()});
        } else {
            setFillHeight(true);
        }
    }
}

VSpacer::VSpacer(const std::optional<float> maxSize)
    : Spacer{maxSize, Orientation::Vertical}
{}

HSpacer::HSpacer(const std::optional<float> maxSize)
    : Spacer{maxSize, Orientation::Horizontal}
{}