struct window{
    struct ToImageRatio{
        float X, Y;
    } toImageRatio;
    struct Transform{
        struct Position{
            float X=0, Y=0;
        } pos;
        struct Scale{
            float Width=1920, Height=1080;
        } scale;
    } transform;
    const int targetFPS = 300;
    void initWin();
    bool isMoving();
    // ConfigFlags state = FLAG_WINDOW_ALWAYS_RUN;
};
