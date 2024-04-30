#if TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME
buildStoryboard,

#elif TEXTURE_EXPOSE == TEXTURE_FILE
__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

#define SB_IMAGE_DIR "data/storyboard/"

char* storyboardFiles[] = {
    SB_IMAGE_DIR "hand-000.png",
    SB_IMAGE_DIR "hand-010-winamp.png",
    SB_IMAGE_DIR "hand-020-title.png",
    SB_IMAGE_DIR "hand-030-transition-mer.png",
    SB_IMAGE_DIR "hand-040-mer.png",
    SB_IMAGE_DIR "hand-050-cote-loin.jpg",
    SB_IMAGE_DIR "hand-060-cote-survol.jpg",
    SB_IMAGE_DIR "hand-065-cote-face.jpg",
    SB_IMAGE_DIR "hand-070-interieur-terres.jpg",
    SB_IMAGE_DIR "hand-075-train.jpg",
    SB_IMAGE_DIR "hand-080-grand-rift-loin.jpg",
    SB_IMAGE_DIR "hand-090-grand-rift.jpg",
    SB_IMAGE_DIR "hand-100-desert.jpg",
    SB_IMAGE_DIR "hand-105-nuit.jpg",
    SB_IMAGE_DIR "hand-110-desert-vivant.jpg",
    SB_IMAGE_DIR "hand-115-desert-vivant.jpg",
    SB_IMAGE_DIR "hand-120-batiments.jpg",
    SB_IMAGE_DIR "hand-140-batiments.jpg",
    SB_IMAGE_DIR "hand-130-batiments.jpg",
    SB_IMAGE_DIR "hand-145-autoroutes.jpg",
    SB_IMAGE_DIR "hand-150-entree-tunnel.jpg",
    SB_IMAGE_DIR "hand-155-rush-tunnel.jpg",
    SB_IMAGE_DIR "hand-156-rush-tunnel-2.jpg",
    SB_IMAGE_DIR "hand-160-rush-batiments.jpg",
    SB_IMAGE_DIR "hand-165-envol.jpg",
    SB_IMAGE_DIR "hand-170-fin.jpg",
};

void buildStoryboard()
{
  assert(ARRAY_LEN(storyboardFiles) == storyboardEnd - storyboardStart);

  for (int i = 0; i < ARRAY_LEN(storyboardFiles); i++)
  {
    Texture::loadTextureFromFile(storyboardFiles[i], (Texture::id)(storyboardStart + i));
  }
}

#endif // TEXTURE_EXPOSE
