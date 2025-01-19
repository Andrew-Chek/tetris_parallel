#ifndef WINDOW_SERVICE_HPP
#define WINDOW_SERVICE_HPP

#include <ncurses.h>
#include <vector>
#include <iostream>
#include <string>
#include <map>

enum ComponentType
{
    Matrix,
    Text
};

class IComponent
{
public:
    int topLeftY;
    int topLeftX;
    ComponentType type;
};

class MatrixComponent : public IComponent
{
public:
    std::vector<std::vector<int>> *matrix;
    MatrixComponent(int topLeftY, int topLeftX, std::vector<std::vector<int>> *matrix)
    {
        this->topLeftY = topLeftY;
        this->topLeftX = topLeftX;
        this->matrix = matrix;
        this->type = ComponentType::Matrix;
    }
};

class TextComponent : public IComponent
{
public:
    char *text;
    TextComponent(int topLeftY, int topLeftX, const char *text)
    {
        this->topLeftY = topLeftY;
        this->topLeftX = topLeftX;
        this->text = (char*) text;
        this->type = ComponentType::Text;
    }
};

class WindowService
{
public:
    WindowService();
    ~WindowService();
    void addComponent(std::vector<std::vector<int>> *board, int y, int x);
    void addComponent(const char* text, int y, int x);
    void render();
    void registerStyle(int key, const char *value);

private:
    std::vector<IComponent *> components;
    std::map<int, const char *> *styleMap;
    void paintBorder(MatrixComponent *component);
    void paintMatrix(MatrixComponent *component);
    void paintText(TextComponent *component);
    void clear();
};

#endif
