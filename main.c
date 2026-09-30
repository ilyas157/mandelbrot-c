#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

typedef struct Complex
{
    double reel;
    double img;
} Complex;

typedef struct
{
    unsigned char r, g, b;
} RGB;

double complex_moduleSquared(Complex *z)
{
    return z->reel * z->reel + z->img * z->img;
}

void complex_squared(Complex *z)
{
    double old_reel = z->reel;
    double old_img = z->img;
    z->reel = old_reel * old_reel - old_img * old_img;
    z->img = 2 * old_reel * old_img;
}

void complex_sum(Complex *z, Complex *k)
{
    z->reel = z->reel + k->reel;
    z->img = z->img + k->img;
}
// Pure math core: given a complex number, how many iterations to escape (smoothed)
double mendelbrot_escape(Complex c, int max_iter)
{
    Complex z = {0.0, 0.0};
    int count = 0;

    while (complex_moduleSquared(&z) < 4 && count < max_iter)
    {
       
         for (int i = 0; i < 1; i++)
        {
            complex_squared(&z);
        }
        complex_sum(&z, &c);
        count++;
    }

    if (count == max_iter)
        return (double)count;

    double modulus = sqrt(complex_moduleSquared(&z));
    return count +1 - log(log(modulus)) / log(2.0);
}

// Maps a (possibly fractional) pixel coordinate to the complex plane, then escapes it
double mendelBrotLoop(Matrix *grid, double x, double y, int max_iter)
{
    Complex c;
    c.reel = -2 + x * (1.0 + 2.0) / (grid->cols - 1);
    c.img = 1.5 + y * (-1.5 - 1.5) / (grid->rows - 1);

    return mendelbrot_escape(c, max_iter);
}

RGB color_map(double smooth_iter, int max_iter)
{
    if (smooth_iter >= max_iter)
        return (RGB){0, 0, 0}; // inside the set: black

    double t = fmod(smooth_iter * 0.05, 1.0);
    unsigned char r = (unsigned char)(9 * (1 - t) * t * t * t * 255);
    unsigned char g = (unsigned char)(15 * (1 - t) * (1 - t) * t * t * 255);
    unsigned char b = (unsigned char)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
    return (RGB){r, g, b};
}

void render_mandelbrot(Uint32 *pixels, int width, int height, int max_iter,
                       double min_re, double max_re, double min_im, double max_im)
{
    for (int py = 0; py < height; py++)
    {
        for (int px = 0; px < width; px++)
        {
            double re = min_re + px * (max_re - min_re) / (width - 1);
            double im = max_im + py * (min_im - max_im) / (height - 1);
            Complex c = {re, im};

            double smooth_iter = mendelbrot_escape(c, max_iter);
            RGB color = color_map(smooth_iter, max_iter);
            pixels[py * width + px] = (color.r << 16) | (color.g << 8) | color.b;
        }
    }
}
// --- inside main(), replace your render/write loop with this ---
int main()
{
    const int WIDTH = 1920, HEIGHT = 1080, MAX_ITERATIONS = 500;
    double min_re = -2.0, max_re = 1.0;
    double min_im = -1.5, max_im = 1.5;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Mandelbrot",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);
    Uint32 *pixels = malloc(WIDTH * HEIGHT * sizeof(Uint32));

    render_mandelbrot(pixels, WIDTH, HEIGHT, MAX_ITERATIONS,
                      min_re, max_re, min_im, max_im);

    // Upload the whole buffer to the texture in one shot
    SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Uint32));

    // Keep the window open until closed
    int running = 1;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mx = event.button.x;
                int my = event.button.y;

                // Convert clicked pixel to complex-plane coordinate (same mapping as render)
                double click_re = min_re + mx * (max_re - min_re) / (WIDTH - 1);
                double click_im = max_im + my * (min_im - max_im) / (HEIGHT - 1);

                double zoom_factor = (event.button.button == SDL_BUTTON_LEFT) ? 0.1 : 10.0;
                // left click = zoom in (shrink bounds), right click = zoom out (grow bounds)

                double re_range = (max_re - min_re) * zoom_factor;
                double im_range = (max_im - min_im) * zoom_factor;

                min_re = click_re - re_range / 2;
                max_re = click_re + re_range / 2;
                min_im = click_im - im_range / 2;
                max_im = click_im + im_range / 2;

                render_mandelbrot(pixels, WIDTH, HEIGHT, MAX_ITERATIONS, min_re, max_re, min_im, max_im);
                SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Uint32));
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    free(pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}