#include <SFML/Graphics.hpp>
#include <cstring>
#include <cmath>
#include <random>
#include <deque>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
//#include "incl/hsl_color.hpp"

#define WIDTH 1280
#define HEIGHT 720
#define PARTICLE_COUNT 5000
#define DISTANCE_DIVIDER 1

static const float gravity_multipier = 100000.0f;

struct Color
{
    int r,g,b,a;
};

float clamp(float val, float min, float max)
{
    if(val<min)
        return min;
    if(val>max)
        return max;
    return val;
}

class Particle{
public:
    sf::Vector2f velocity;
    //The number of this particle
    sf::Vector2f position;
    Color color;
    Particle(){}
    Particle(std::random_device &dRE, std::uniform_real_distribution<float> &rand_dist)
    {
        position.x = rand_dist(dRE)*WIDTH;
        position.y = rand_dist(dRE)*HEIGHT;
        velocity.x = 0;
        velocity.y = 0;
        color = {(int)(rand_dist(dRE)*255.0f),
                 (int)(rand_dist(dRE)*255.0f),
                 (int)(rand_dist(dRE)*255.0f),
                 255};
    }

    void tick(float tslf, const sf::Vector2i &cursor, float mass)
    {
            
        sf::Vector2f posOther((float)cursor.x, (float)cursor.y);
        sf::Vector2f difference = posOther - position;
        float magnitude = sqrt(pow(difference.x,2) + pow(difference.y,2));
        float gravity = gravity_multipier * mass *(1/pow(magnitude*DISTANCE_DIVIDER,2));
        sf::Vector2f deltaV = difference / magnitude * gravity * tslf;
        if(magnitude < 1)
            deltaV *= -2.0f;
        velocity = deltaV + velocity *0.95f;
        position += velocity;
        if(position.x < 0)
        {
            velocity.x *= -1.0f;
            //velocity.y = 0;
        }
        if(position.y < 0)
        {
            //velocity.x = 0;
            velocity.y *= -1.0f;
        }
        if(position.x > WIDTH)
        {
            velocity.x *= -1.0f;
            //velocity.y = 0;
        }
        if(position.y > HEIGHT)
        {
            //velocity.x = 0;
            velocity.y *= -1.0f;
        }
    }
};

void setPixel(sf::Uint8 *pixels, int width, int height, int x, int y, int r, int g, int b, int a)
{
    if(y>height || x > width || x<0 || y<0)
        return;
    pixels[4*(width*y + x)] = r;
    pixels[4*(width*y + x) + 1] = g;
    pixels[4*(width*y + x) + 2] = b;
    pixels[4*(width*y + x) + 3] = a;
}

int main()
{

    auto rE = new std::random_device;
    auto rD = new std::uniform_real_distribution<float>(0.0,1.0);

    //Particle *particles = new Particle[PARTICLE_COUNT];
    std::vector<Particle> particles(PARTICLE_COUNT);
    for(int i = 0; i < (int)(particles.size()); i++)
        particles[i] = Particle(*rE, *rD);
    sf::RenderWindow window(sf::VideoMode(WIDTH,
                                          HEIGHT) , "_DEV_ SFML testing");

    sf::Uint8* pixels = new sf::Uint8[WIDTH * HEIGHT * 4];
    sf::Texture texture;
    sf::Sprite sprite;
    texture.create(WIDTH, HEIGHT);
    float tslf = 0.0f;
    sprite.setTexture(texture);
    sprite.setScale(sf::Vector2f(1.0f, 1.0f));
    sprite.setPosition(sf::Vector2f(0.0f, 0.0f));
    float tp = 0.0f;
    while (window.isOpen())
    {

        std::clock_t begin = clock();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        memset(pixels,0,(sizeof pixels[0]) * WIDTH*HEIGHT*4);
        sf::Vector2i cursor;
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            cursor = sf::Mouse::getPosition(window);
            tp += tslf;
        } else
        {
            tp = 0.0f;
        }
        float mass = pow(tp*2,2);
        for(int i = 0; i < (int)(particles.size()); i++)
        {

            setPixel(pixels, WIDTH, HEIGHT, particles[i].position.x, particles[i].position.y,
                     particles[i].color.r,
                     particles[i].color.g,
                     particles[i].color.b,
                     particles[i].color.a);
            particles[i].tick(tslf, cursor, mass);
        }
        texture.update(pixels);
        window.draw(sprite);
        window.display();
        if(tslf < 1.0f/60.0f)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0f/60.0f-tslf*1000.0f)));
        }
        std::clock_t end = clock();
        tslf = float(end - begin) / CLOCKS_PER_SEC;
    }

    delete rE;
    delete rD;
    delete pixels;
    return 0;
}
