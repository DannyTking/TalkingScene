//
//  Tweens.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _TWEENS_H_
#define _TWEENS_H_

#include <string>

namespace Walaber
{
    // Tweens taken from Rober Penner's Tweening Equations
    // http://www.robertpenner.com/easing/penner_chapter7_tweening.pdf
    typedef float (*EasingFunction)(float, float, float, float);
    class Tweens
    {
    public:
     
        enum Type
        {
            Type_Linear,
            Type_QuadraticEaseIn,
            Type_QuadraticEaseOut,
            Type_QuadraticEaseInOut,
            Type_QuadraticEaseOutIn,
            Type_CubicEaseIn,
            Type_CubicEaseOut,
            Type_CubicEaseInOut,
            Type_CubicEaseOutIn,
            Type_QuarticEaseIn,
            Type_QuarticEaseOut,
            Type_QuarticEaseInOut,
            Type_QuarticEaseOutIn,
            Type_QuinticEaseIn,
            Type_QuinticEaseOut,
            Type_QuinticEaseInOut,
            Type_QuinticEaseOutIn,
        };
        
        // returns an easing function for a string name and value
        static EasingFunction getEasingFunctionForString(const std::string& easeName, const int value);
        
        // give the pointer to an easing function, this function returns an enum representing that function
        static Type getTweenTypeForEasingFunction(EasingFunction ease);
        
        // give the pointer to an easing function, this function returns an enum representing that function
        static EasingFunction getEasingFunctionForTweenType(Type type);
        
        // Linear tween
        static inline float linearTween(float time, float beginning, float change, float duration)
        {
            return change * time / duration + beginning;
        }
        
        // Quadratic tweens
        static inline float quadraticEaseIn(float time, float beginning, float change, float duration)
        {
            return change * ((time /= duration) * time) + beginning;
        }
        
        static inline float quadraticEaseOut(float time, float beginning, float change, float duration)
        {
            
            return -change * (time /= duration) * (time - 2.0f) + beginning;
        }
        
        static inline float quadraticEaseInOut(float time, float beginning, float change, float duration)
        {            
            if ((time /= (duration/2.0f)) < 1.0f) 
            {
                return change/2.0f*time*time + beginning;
            }
            
            return -change/2.0f * ((--time)*(time-2.0f) - 1.0f) + beginning;
        }
        
        static inline float quadraticEaseOutIn(float time, float beginning, float change, float duration)
        {            
            if ((time /= (duration/2.0f)) < 1.0f) 
            {
                return -change/2.0f * ((time)*(time-2.0f)) + beginning;
            }
            
            return change/2.0f * ((--time*time) + 1.0f ) + beginning;
        }
        
        // Cubic tweens
        static inline float cubicEaseIn(float time, float beginning, float change, float duration)
        {            
            return change * ((time /= duration) * time * time) + beginning;
        }
        
        static inline float cubicEaseOut(float time, float beginning, float change, float duration)
        {
            return change * ((((time /= duration) -= 1.0f) * time * time)+1.0f) + beginning;
        }
        
        static inline float cubicEaseInOut(float time, float beginning, float change, float duration)
        {                       
            if ( ( time /= (duration/2.0f) ) < 1.0f )
            {
                return change/2.0f * (time*time*time) + beginning;
            }
            
            return change/2.0f * (((time -= 2.0f) * time * time) + 2.0f) + beginning;
        }
        
        static inline float cubicEaseOutIn(float time, float beginning, float change, float duration)
        {                       
            if ( ( time /= (duration/2.0f) ) < 1.0f )
            {
                return change/2.0f * (((--time) * time * time) + 1.0f) + beginning;
            }
            
            return change/2.0f * ((--time*time*time) + 1.0f) + beginning;
        }
        
        // Quartic tweens
        static inline float quarticEaseIn(float time, float beginning, float change, float duration)
        {
            
            return change * ((time /= duration) * time * time * time) + beginning;
        }
        
        static inline float quarticEaseOut(float time, float beginning, float change, float duration)
        {
            
            return -change * ((((time /= duration) -= 1.0f) * time * time * time) - 1.0f) + beginning;
        }
        
        static inline float quarticEaseInOut(float time, float beginning, float change, float duration)
        {
            
            if ( ( time /= (duration/2.0f) ) < 1.0f )
            {
                return change/2.0f * (time*time*time*time) + beginning;
            }
            
            return -change/2.0f * (((time -= 2.0f) * time * time * time) - 2.0f) + beginning;
        }
        
        static inline float quarticEaseOutIn(float time, float beginning, float change, float duration)
        {
            
            if ( ( time /= (duration/2.0f) ) < 1.0f )
            {
                return -change/2.0f * ((--time * time * time * time) - 1.0f) + beginning;
            }
            
            return change/2.0f * ((--time*time*time*time) + 1.0f) + beginning;
        }
        
        // Quintic tweens
        static inline float quinticEaseIn(float time, float beginning, float change, float duration)
        {
            
            return change * ((time /= duration) * time * time * time * time) + beginning;
        }
        
        static inline float quinticEaseOut(float time, float beginning, float change, float duration)
        {
            
            return change * ((((time /= duration) -= 1.0f) * time * time * time * time) + 1.0f) + beginning;
        }
        
        static inline float quinticEaseInOut(float time, float beginning, float change, float duration)
        {            
            if ( ( time /= (duration/2.0f) ) < 1.0f )
            {
                return change/2.0f * (time*time*time*time*time) + beginning;
            }
            
            return change/2.0f * (((time -= 2.0f) * time * time * time * time) + 2.0f) + beginning;
        }
        
        static inline float quinticEaseOutIn(float time, float beginning, float change, float duration)
        {            
            if ( ( time /= (duration/2.0f) ) < 1.0f )
            {
                return change/2.0f * ((--time * time * time * time * time) + 1.0f) + beginning;
            }
            
            return change/2.0f * ((--time*time*time*time*time) + 1.0f) + beginning;
        }
        
    };
}

#endif

