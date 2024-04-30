
#if TEXTURE_EXPOSE == TEXTURE_NAMES

titleScreen,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildTitleScreen,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

char* imageFiles[] = {
    IMAGE_DIR "title-screen.png",
    IMAGE_DIR "title-screen-glow.png",
};

void buildTitleScreen()
{
  Texture::loadTextureFromFile(imageFiles[0], Texture::titleScreen);
  Texture::loadTextureFromFile(imageFiles[1], Texture::titleScreenGlow);
}

#endif // TEXTURE_EXPOSE
