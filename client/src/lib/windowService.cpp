#include "../../include/lib/windowService.hpp"

#define BORDER_WIDTH 1

WindowService::WindowService()
{
    styleMap = new std::map<int, const char *>();
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    components = std::vector<IComponent *>();
}
WindowService::~WindowService()
{
    endwin();
}

void WindowService::addComponent(std::vector<std::vector<int>> *matrix, int y, int x)
{
    std::vector<std::vector<int>> *squareMatrix = new std::vector<std::vector<int>>((*matrix).size(), std::vector<int>((*matrix)[0].size() * 2, 0));
    for (int i = 0; i < (int)(*squareMatrix).size(); ++i)
    {
        for (int j = 0; j < (int)(*squareMatrix)[0].size(); j += 2)
        {
            (*squareMatrix)[i][j] = (*matrix)[i][j / 2];
            (*squareMatrix)[i][j + 1] = (*matrix)[i][j / 2];
        }
    }
    MatrixComponent *component = new MatrixComponent(y, x, squareMatrix);
    components.push_back(component);
}

void WindowService::addComponent(const char* text, int y, int x)
{
    TextComponent *component = new TextComponent(y, x, text);
    components.push_back(component);
}

void WindowService::render()
{
    erase();
    for (IComponent *icomponent : components)
    {
        switch (icomponent->type)
        {
        case ComponentType::Matrix:
            paintMatrix((MatrixComponent *)icomponent);
            break;
        case ComponentType::Text:
            paintText((TextComponent *)icomponent);
            break;
        }
    }
    refresh();
    clear();
}

void WindowService::paintMatrix(MatrixComponent *component)
{
    for (int i = 0; i < (int)(*(*component).matrix).size(); ++i)
    {
        for (int j = 0; j < (int)(*(*component).matrix)[i].size(); ++j)
        {
            int key = (*(*component).matrix)[i][j];
            const char *value = " ";
            if (styleMap->find(key) != styleMap->end())
            {
                value = (*styleMap)[key];
            }
            mvaddstr(component->topLeftY + i, component->topLeftX + j, value);
        }
    }
    paintBorder(component);
}

void WindowService::paintBorder(MatrixComponent *component)
{
    int y = (*component).topLeftY;
    int x = (*component).topLeftX;
    int height = (*component->matrix).size();
    int width = (*component->matrix)[0].size();
    mvhline(y - 1, x, 0, width);
    mvhline(y + height, x, 0, width);
    mvvline(y, x - 1, 0, height);
    mvvline(y, x + width, 0, height);
    mvaddch(y - 1, x - 1, ACS_ULCORNER);
    mvaddch(y + height, x - 1, ACS_LLCORNER);
    mvaddch(y - 1, x + width, ACS_URCORNER);
    mvaddch(y + height, x + width, ACS_LRCORNER);
}

void WindowService::paintText(TextComponent* component) 
{
    mvaddstr(component->topLeftY, component->topLeftX, component->text);
}

void WindowService::clear()
{
    components.clear();
}

void WindowService::registerStyle(int key, const char *value)
{
    (*styleMap)[key] = value;
}
