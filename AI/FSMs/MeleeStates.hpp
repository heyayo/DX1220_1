#ifndef DX1220_MELEESTATES_HPP
#define DX1220_MELEESTATES_HPP

#include "Fsm.hpp"

namespace MELEESTATES
{
    struct SearchingState : public STATE
    {
        virtual void Update(double deltaTime) override;
        virtual STATE* StateChanger() override;
    };
}

#endif
