//
//  Tweens.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Tweens.h"

namespace Walaber
{
    EasingFunction Tweens::getEasingFunctionForString(const std::string& easeName, const int value)
    {
        if(easeName == "quadratic")
        {
            if(value < 0)
            {
                return &quadraticEaseIn;
            }
            else if(value > 0)
            {
                return &quadraticEaseOut;
            }
            else
            {
                return &linearTween;
            }
        }
        else if(easeName == "cubic")
        {
            if(value < 0)
            {
                return &cubicEaseIn;
            }
            else if(value > 0)
            {
                return &cubicEaseOut;
            }
            else
            {
                return &linearTween;
            }
        }
        else if(easeName == "quartic")
        {
            if(value < 0)
            {
                return &quarticEaseIn;
            }
            else if(value > 0)
            {
                return &quarticEaseOut;
            }
            else
            {
                return &linearTween;
            }
        }
        else if(easeName == "quintic")
        {
            if(value < 0)
            {
                return &quinticEaseIn;
            }
            else if(value > 0)
            {
                return &quinticEaseOut;
            }
            else
            {
                return &linearTween;
            }
        }
        else if(easeName == "dual quadratic")
        {
            if(value < 0)
            {
                return &quadraticEaseInOut;
            }
            else if(value > 0)
            {
                return &quadraticEaseOutIn;
            }
            else
            {
                return &linearTween;
            }
        }
        else if(easeName == "dual cubic")
        {
            if(value < 0)
            {
                return &cubicEaseInOut;
            }
            else if(value > 0)
            {
                return &cubicEaseOutIn;
            }
            else
            {
                return &linearTween;
            }
        }
        else if(easeName == "dual quartic")
        {
            if(value < 0)
            {
                return &quarticEaseInOut;
            }
            else if(value > 0)
            {
                return &quarticEaseOutIn;
            }
            else
            {
                return &linearTween;
            }
        }
        else if(easeName == "dual quintic")
        {
            if(value < 0)
            {
                return &quinticEaseInOut;
            }
            else if(value > 0)
            {
                return &quinticEaseOutIn;
            }
            else
            {
                return &linearTween;
            }
        }
        
        return &linearTween;
    }
    
    Tweens::Type Tweens::getTweenTypeForEasingFunction(EasingFunction ease)
    {
        if(ease == &linearTween)
        {
            return Type_Linear;
        }
        else if(ease == &quadraticEaseIn)
        {
            return Type_QuadraticEaseIn;
        }
        else if(ease == &quadraticEaseOut)
        {
            return Type_QuadraticEaseOut;
        }
        else if(ease == &quadraticEaseInOut)
        {
            return Type_QuadraticEaseInOut;
        }
        else if(ease == &quadraticEaseOutIn)
        {
            return Type_QuadraticEaseOutIn;
        }
        else if(ease == &cubicEaseIn)
        {
            return Type_CubicEaseIn;
        }
        else if(ease == &cubicEaseOut)
        {
            return Type_CubicEaseOut;
        }
        else if(ease == &cubicEaseInOut)
        {
            return Type_CubicEaseInOut;
        }
        else if(ease == &cubicEaseOutIn)
        {
            return Type_CubicEaseOutIn;
        }
        else if(ease == &quarticEaseIn)
        {
            return Type_QuarticEaseIn;
        }
        else if(ease == &quarticEaseOut)
        {
            return Type_QuarticEaseOut;
        }
        else if(ease == &quarticEaseInOut)
        {
            return Type_QuarticEaseInOut;
        }
        else if(ease == &quarticEaseOutIn)
        {
            return Type_QuarticEaseOutIn;
        }
        else if(ease == &quinticEaseIn)
        {
            return Type_QuinticEaseIn;
        }
        else if(ease == &quinticEaseOut)
        {
            return Type_QuinticEaseOut;
        }
        else if(ease == &quinticEaseInOut)
        {
            return Type_QuinticEaseInOut;
        }
        else if(ease == &quinticEaseOutIn)
        {
            return Type_QuinticEaseOutIn;
        }
        
        return Type_Linear;
    }
    
    // give the pointer to an easing function, this function returns an enum representing that function
    EasingFunction Tweens::getEasingFunctionForTweenType(Tweens::Type type)
    {
        if(type == Type_Linear)
        {
            return &linearTween;
        }
        else if(type == Type_QuadraticEaseIn)
        {
            return &quadraticEaseIn;
        }
        else if(type == Type_QuadraticEaseOut)
        {
            return &quadraticEaseOut;
        }
        else if(type == Type_QuadraticEaseInOut)
        {
            return &quadraticEaseInOut;
        }
        else if(type == Type_QuadraticEaseOutIn)
        {
            return &quadraticEaseOutIn;
        }
        else if(type == Type_CubicEaseIn)
        {
            return &cubicEaseIn;
        }
        else if(type == Type_CubicEaseOut)
        {
            return &cubicEaseOut;
        }
        else if(type == Type_CubicEaseInOut)
        {
            return &cubicEaseInOut;
        }
        else if(type == Type_CubicEaseOutIn)
        {
            return &cubicEaseOutIn;
        }
        else if(type == Type_QuarticEaseIn)
        {
            return &quarticEaseIn;
        }
        else if(type == Type_QuarticEaseOut)
        {
            return &quarticEaseOut;
        }
        else if(type == Type_QuarticEaseInOut)
        {
            return &quarticEaseInOut;
        }
        else if(type == Type_QuarticEaseOutIn)
        {
            return &quarticEaseOutIn;
        }
        else if(type == Type_QuinticEaseIn)
        {
            return &quinticEaseIn;
        }
        else if(type == Type_QuinticEaseOut)
        {
            return &quinticEaseOut;
        }
        else if(type == Type_QuinticEaseInOut)
        {
            return &quinticEaseInOut;
        }
        else if(type == Type_QuinticEaseOutIn)
        {
            return &quinticEaseOutIn;
        }
        
        return &linearTween;
    }
}