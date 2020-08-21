#ifndef TEXT_HPP
#define TEXT_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class TextManager;
class Camera;
extern SDL_Renderer *_renderer;

enum TextAlignFlag {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_BOTTOM,
    ALIGN_TOP
};

class Text {
public:
    Text(TTF_Font *font) {
        mFont = font;
        mPropertiesUpdated = true;
    }
    
    void setText(std::string text) {
        mTextString = text;
        mPropertiesUpdated = true;
    }
    
    void setPosition(int xPos, int yPos) {
        dstRect.x = xPos;
        dstRect.y = yPos;
        mPropertiesUpdated = true;
    }
    
    void setTextOffset(int xOffset, int yOffset) {
        mOffset[0] = xOffset;
        mOffset[1] = yOffset;
        mPropertiesUpdated = true;
    }
    
    void setTextAlignment(TextAlignFlag xAlign, TextAlignFlag yAlign=ALIGN_TOP) {
        mAlignment[0] = xAlign;
        mAlignment[1] = yAlign;
    }
    
    void setDimensions(int xDim, int yDim) {
        srcRect.w = xDim;
        srcRect.h = yDim;
        dstRect.w = xDim;
        dstRect.h = yDim;
        mPropertiesUpdated = true;
    }
    
    void draw(Camera *camera) {
        if (mPropertiesUpdated) {
            SDL_Surface *mTextSurface = TTF_RenderText_Solid(mFont, mTextString.c_str(), colour);
            
            switch (mAlignment[0]) {
                case ALIGN_RIGHT:
                    srcRect.x = -(srcRect.w - mTextSurface->w + mOffset[0]);
                    break;
                case ALIGN_CENTER:
                    srcRect.x = -((srcRect.w - mTextSurface->w) / 2 + mOffset[0]);
                    break;
                default:
                    srcRect.x = -mOffset[0];
            }
            
            switch (mAlignment[0]) {
                case ALIGN_BOTTOM:
                    srcRect.y = -(srcRect.h - mTextSurface->h + mOffset[1]);
                    break;
                case ALIGN_CENTER:
                    srcRect.y = -((srcRect.h - mTextSurface->h) / 2 + mOffset[1]);
                    break;
                default:
                    srcRect.y = -mOffset[1];
            }
            
            mTextTexture = SDL_CreateTextureFromSurface(_renderer, mTextSurface);
            
            mPropertiesUpdated = false;
        }
        
        SDL_RenderCopy(_renderer, mTextTexture, &srcRect, &dstRect);
    }
private:
    TTF_Font *mFont;
    
    std::string mTextString;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int mOffset[2];
    SDL_Color colour;
    TextAlignFlag mAlignment[2];

    SDL_Texture *mTextTexture;
    bool mPropertiesUpdated;
};

#endif