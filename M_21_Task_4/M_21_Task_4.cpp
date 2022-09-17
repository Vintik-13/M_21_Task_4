/*Задание 4. Реализация ролевой пошаговой игры (дополнительное задание)
Что нужно сделать
Реализуйте сильно упрощённую версию ролевой пошаговой игры.Игра происходит
на карте размером 20 на 20 клеток. По клеткам перемещаются враги и персонаж игрока.
После каждого хода игрока карта показывается вновь со всеми врагами на ней. Игрок
помечается буквой P, враги — буквой E, пустые места — точкой. 
Каждый персонаж игры представлен в виде структуры с полями: имя, жизни, броня, урон.
Вначале игры создаются 5 случайных врагов в случайных клетках карты. Имена врагам 
задаются в формате Enemy #N, где N — это порядковый номер врага. Уровень жизней
врагам задаётся случайно — от 50 до 150. Уровень брони варьируется от 0 до 50.
Урон тоже выбирается случайно — от 15 до 30 единиц.
Игрок конструирует своего персонажа самостоятельно. Задаёт все его параметры,
включая имя.
Все персонажи появляются в случайных местах карты.
Игрок осуществляет ход с помощью следующих команд: w, a, s, d. В зависимости от команды
выбирается направление перемещения персонажа: влево, вправо, вверх, вниз.
Враги перемещаются в случайном направлении.
Если персонаж (враг или игрок) перемещается в сторону, где уже находится какой-то персонаж,
то он бьёт этого персонажа с помощью своего урона. Враги при этом никогда не бьют врагов,
а просто пропускают ход и остаются на своём месте. За пределы карты (20 на 20 клеток) ходить
нельзя никому. Если кто-то выбрал направление за гранью дозволенного, ход пропускается.
Формула для расчёта урона совпадает с той, что была в занятии. Жизни уменьшаются на
оставшийся после брони урон. При этом броня тоже сокращается на приведённый урон.
Игра заканчивается тогда, когда либо умирают все враги, либо персонаж игрока. В первом
случае на экран выводится сообщение о поражении, во втором — победа.
Если в начале хода игрок вводит команду save или load вместо направления перемещения,
то игра либо делает сохранение своего состояния в файл, либо загружает это состояние из
файла соответственно.*/

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <fstream>

namespace constants {
    const int MAX_MAP = 20;
    const int COUNT_PLAYER = 6;
    const char NAME_PLAUER = 'P';
    const char NAME_ENEMY = 'E';
    const int MIN_HEALTH = 50;
    const int MAX_HEALTH = 150;
    const int MIN_ARMOR = 0;
    const int MAX_ARMOR = 50;
    const int MIN_DAMAGE = 15;
    const int MAX_DAMAGE = 30;
    const std::string DIR = "Savegame";
    const std::string FILE = "save.bin";
}

//Структура вектор места игрока на поле
struct placePlayer {
    int x{ 0 };
    int y{ 0 };
};

//Структура вектор направления движения игрока
struct moovePlayer {
    int x{ 0 };
    int y{ 0 };
};

//Структура игрок
struct player {
    std::string name{"E"};
    int health{ 0 };
    int armor{ 0 };
    int damage{ 0 };
    placePlayer place;
};

//Функция генерации исходного места врага
void creatPlaceEnemy(bool* playing_field, player& enemy) {
    *(playing_field + (enemy.place.x * enemy.place.y)) = true;
    while (true) {        
        enemy.place.x = 0 + std::rand() % constants::MAX_MAP;
        enemy.place.y = 0 + std::rand() % constants::MAX_MAP;
        if (*(playing_field + enemy.place.x + enemy.place.y) == false) //Пока не попадем на пустую клетку
            break;
    }   
}

//Валидация ввода числа в указанном диапазоне используется в creatHero
int validNumber(std::string const& str, int min, int max) {
    
    auto aa = str.empty();
    auto bb = str.find_first_not_of("0123456789");
    auto cc = str.find_first_not_of("0123456789") != std::string::npos;
    int dd = std::string::npos;

    if (str.empty() || str.find_first_not_of("0123456789") != std::string::npos)
        return -1;

    int tmp = std::stoi(str);//Здесь нет необходимости применять try catch, проверка на цыфры выше
    return tmp >= min && tmp <= max ? tmp : -1;
}

//Функция инициализации героя
void creatHero(player* enemy) {
    std::cout << "Your hero is indicated by the letter \"P\"\n";
    enemy->name = 'P';
    std::string ini;
    std::cout << "Set the hero's health in the range from 50 to 150: ";
    std::getline(std::cin, ini);
    enemy->health = validNumber(ini, constants::MIN_HEALTH, constants::MAX_HEALTH);
    while (enemy->health == -1) {
        std::cout << "Incorrect input, try again!\n";
        std::getline(std::cin, ini);
        enemy->health = validNumber(ini, constants::MIN_HEALTH, constants::MAX_HEALTH);
    }
    std::cout << "Set the hero's armor in the range from 0 to 50: ";
    std::getline(std::cin, ini);
    enemy->armor = validNumber(ini, constants::MIN_ARMOR, constants::MAX_ARMOR);
    while (enemy->armor == -1) {
        std::cout << "Incorrect input, try again!\n";
        std::getline(std::cin, ini);
        enemy->armor = validNumber(ini, constants::MIN_ARMOR, constants::MAX_ARMOR);
    }
    std::cout << "Set the hero's damage in the range from 15 to 30: ";
    std::getline(std::cin, ini);
    enemy->damage = validNumber(ini, constants::MIN_DAMAGE, constants::MAX_DAMAGE);
    while (enemy->damage == -1) {
        std::cout << "Incorrect input, try again!\n";
        std::getline(std::cin, ini);
        enemy->damage = validNumber(ini, constants::MIN_DAMAGE, constants::MAX_DAMAGE);
    }

    std::cout << "Enter the X coordinate (from 1 to 20): ";
    std::getline(std::cin, ini);
    enemy->place.x = validNumber(ini, 1, constants::MAX_MAP);
    while (enemy->place.x == -1) {
        std::cout << "Incorrect input, try again!\n";
        std::getline(std::cin, ini);
        enemy->place.x = validNumber(ini, constants::MIN_DAMAGE, constants::MAX_DAMAGE);
    }
    enemy->place.x -= 1;
    std::cout << "Enter the Y coordinate (from 1 to 20): ";
    std::getline(std::cin, ini);
    enemy->place.y = validNumber(ini, 1, constants::MAX_MAP);
    while (enemy->place.y == -1) {
        std::cout << "Incorrect input, try again!\n";
        std::getline(std::cin, ini);
        enemy->place.y = validNumber(ini, constants::MIN_DAMAGE, constants::MAX_DAMAGE);
    }
    enemy->place.y -= 1;    
}

//Функция генерации случайного врага
void creatEnemy(bool* playing_field, int map, player* enemy, int size) {
    for (int i = 1; i < size; i++) {
        (enemy + i)->name += std::to_string(i);
        (enemy + i)->health = (constants::MIN_HEALTH + std::rand() % (constants::MAX_HEALTH - constants::MIN_HEALTH + 1)) ;
        (enemy + i)->armor = (constants::MIN_ARMOR + std::rand() % (constants::MAX_ARMOR - constants::MIN_ARMOR + 1)) ;
        (enemy + i)->damage = (constants::MIN_DAMAGE + std::rand() % (constants::MAX_DAMAGE - constants::MIN_DAMAGE + 1));
        creatPlaceEnemy(playing_field, *(enemy + i));
    }
}

//Функция возвращает имя игрока на игровом поле или '.' исползуется в showMap
std::string placeCharacter(player* enemy, int size_enemy, int x, int y) { 
    std::string tmp;
    for (int i = 0; i < size_enemy; i++) {
        if ((enemy + i)->place.x == x && (enemy + i)->place.y == y) {
            if(i == 0)
                return enemy->name + " ";
            else
                return (enemy + i)->name;
        }
        else
            tmp = ". ";
    }   
    return tmp;
}

//Функция выводит в консоль игровое поле с игроками 
void showMap(player* enemy, int size_enemy, std::string attack_player = "", std::string attack_enemy = "") {
    std::system("mode con cols=100 lines=40");
    std::system("cls");
    for (int i = 0; i < size_enemy; i++) {
        std::cout
            << std::setw(3) << std::left << (enemy + i)->name << " "
            << std::setw(7) << std::left << "Health" << std::setw(4) << std::right << (enemy + i)->health << " "
            << std::setw(7) << std::left << "Armor" << std::setw(4) << std::right << (enemy + i)->armor << " "
            << std::setw(7) << std::left << "Damage" << std::setw(4) << std::right << (enemy + i)->damage << " "
            << "\n";
    }
    std::cout << "\n";           

    for (int y = 0; y < constants::MAX_MAP; y++) {
        for (int x = 0; x < constants::MAX_MAP; x++) {
            std::cout << placeCharacter(enemy, size_enemy, x, y);
        }
        std::cout << std::endl;
    }
    std::cout << "\n";
    if (!attack_player.empty())
        std::cout << attack_player << std::endl;
    if (!attack_enemy.empty())
        std::cout << attack_enemy << std::endl;
}

//Функция выводит в консоль результат используется в showResult
void showResult( player* enemy, int size_enemy, std::string attack_player, std::string attack_enemy,int flag = 0) {
    switch (flag) {    
    case 1:
        std::system("cls");
        std::cout << "\nYou've lost!\n";
        break;
    case 2:
        std::system("cls");
        std::cout << "\nYou have won!!!!!!!!!!!\n";
        break;
    default:
        showMap(enemy, size_enemy, attack_player, attack_enemy);
        break;
    }
}

//Генерация случайного вектора направления
moovePlayer randomMoove() {
    moovePlayer tmp;
    int vec{ 0 };
    vec = 1 + rand() % 2;
    //switch для случайного выбора направления (х или y)
    //В условии нет четкой инструкции по этому поводу
    //Я принял условие при котором игрок не может ходить по диагонали
    switch (vec) 
    {
    case 1:
        while (!tmp.x) 
            tmp.x = -1 + rand() % 3; 
        break;
    case 2:
        while (!tmp.y)            
            tmp.y = -1 + rand() % 3; 
        break;   
    }
    while (!tmp.x && !tmp.y) {
        tmp.x = -1 + rand() % 2;
        tmp.y = -1 + rand() % 2;
    }
    return tmp;
}

//Функция сложения вектора места и вектора направления игрока на поле
void addVectors(placePlayer& a, const moovePlayer& b) {
    a.x = a.x + b.x;
    a.y = a.y + b.y;
    if (a.x < 0) a.x = 0;
    if (a.y < 0) a.y = 0;
    if (a.x > constants::MAX_MAP - 1) a.x = constants::MAX_MAP - 1;
    if (a.y > constants::MAX_MAP - 1) a.y = constants::MAX_MAP - 1;
}

//Функция результат сражения двух игроков
void battleResult(player* a, const player* b) {
    std::cout << std::endl << std::endl << b->name << "-->" << a->name << std::endl << std::endl;
    a->armor -= b->damage;
    if (a->armor < 0) {
        a->health += a->armor;
        a->armor = 0;
    }
}

//Функция изменения массива игроков при уничтожении одного из низ
/*Я зная, что в случае с динамической памятью я должен получить
  указатель на массив, удалить из него элемент, создать новый массив
  и вернуть на него указатель. Но в случае со стеком и статической
  памятью я это реализовать не смог. Поетому все по старинке, указатель
  прежний, казмер массива прехний, просто уменьшид int size*/
int resizePlayerArray(player* enemy, int& enemy_size) {
    int count{ 0 };
    int p{ 0 };
    for (int i = 0; i < enemy_size; i++) {
        count += (enemy + i)->health <= 0 ? 1 : 0;
        p += enemy->health <= 0 ? 1 : 0;
    }
       
    int enemy_size_tmp = enemy_size - count;
    if (p > 0) return 1;
    else if (enemy_size_tmp <= 1) return 2;
    else if (count > 0) { 
        int pos;
        for (int i = 0; i < enemy_size; i++) {
            if ((enemy + i)->health <= 0) {
                pos = i;
                for (int j = pos; j < enemy_size - 1; j++) {                    
                        *(enemy + j) = *(enemy + j + 1);
                }
                break;
            }            
        }        
        enemy_size--;
        return 0;
    }
    else return 0;    
}

//Функция реализующая ход героя
std::string playersMove(int option, player* enemy, int enemy_size) { 
    std::string tmp = "";
    switch (option)
    {
    case 0:
        enemy->place.x -= 1;
        if(enemy->place.x < 0 || enemy->place.x > 19)
            enemy->place.x += 1;
        for (int i = 1; i < enemy_size; i++) {
            if (enemy->place.x == (enemy + i)->place.x && enemy->place.y == (enemy + i)->place.y) {
                tmp = "Attack " + enemy->name + " --> " + (enemy + i)->name + "\n";
                battleResult((enemy + i), enemy);
                enemy->place.x = (enemy + i)->health <= 0 ? (enemy + i)->place.x : enemy->place.x + 1;
                enemy->place.y = (enemy + i)->health <= 0 ? (enemy + i)->place.y : enemy->place.y;
            }            
        }
        break;
    case 1: 
        enemy->place.x += 1;
        if (enemy->place.x < 0 || enemy->place.x > 19)
            enemy->place.x -= 1;
        for (int i = 1; i < enemy_size; i++) {
            if (enemy->place.x == (enemy + i)->place.x && enemy->place.y == (enemy + i)->place.y) {
                tmp = "Attack " + enemy->name + " --> " + (enemy + i)->name;
                battleResult((enemy + i), enemy);
                enemy->place.x = (enemy + i)->health <= 0 ? (enemy + i)->place.x : enemy->place.x - 1;
                enemy->place.y = (enemy + i)->health <= 0 ? (enemy + i)->place.y : enemy->place.y;
            }
        }
        break;
    case 2:
        enemy->place.y += 1;
        if (enemy->place.y < 0 || enemy->place.y > 19)
            enemy->place.y -= 1;
        for (int i = 1; i < enemy_size; i++) {
            if (enemy->place.x == (enemy + i)->place.x && enemy->place.y == (enemy + i)->place.y) {
                tmp = "Attack " + enemy->name + " --> " + (enemy + i)->name;
                battleResult((enemy + i), enemy);
                enemy->place.x = (enemy + i)->health <= 0 ? (enemy + i)->place.x : enemy->place.x;
                enemy->place.y = (enemy + i)->health <= 0 ? (enemy + i)->place.y : enemy->place.y - 1;
            }
        }
        break;
    case 3:
        enemy->place.y -= 1;
        if (enemy->place.y < 0 || enemy->place.y > 19)
            enemy->place.y += 1;
        for (int i = 1; i < enemy_size; i++) {
            if (enemy->place.x == (enemy + i)->place.x && enemy->place.y == (enemy + i)->place.y) {
                tmp = "Attack " + enemy->name + " --> " + (enemy + i)->name;
                battleResult((enemy + i), enemy);
                enemy->place.x = (enemy + i)->health <= 0 ? (enemy + i)->place.x : enemy->place.x;
                enemy->place.y = (enemy + i)->health <= 0 ? (enemy + i)->place.y : enemy->place.y + 1;
            }
        }
        break;    
    }
    return tmp;
}

//В одном месте не могут находится два врага используется в enemyMove
bool posEnemy(player* a, player* enemy, int enemy_size) {
    for (int i = 1; i < enemy_size; i++) {
        if (a->name != (enemy + i)->name)
            if (a->place.x == (enemy + i)->place.x && a->place.y == (enemy + i)->place.y)
                return false;
    }
    return true;
}

//Функция реализующая рандомные ходы противников
std::string enemyMove(player* enemy, int enemy_size) {
    std::string temp = "";
    for (int i = 1; i < enemy_size; i++) {
        placePlayer tmp = (enemy + i)->place;
        addVectors((enemy + i)->place, randomMoove());
        if ((enemy + i)->place.x < 0 || (enemy + i)->place.x > 19 || (enemy + i)->place.y < 0 || (enemy + i)->place.y > 19)
            (enemy + i)->place = tmp;
        if(!posEnemy((enemy + i), enemy, enemy_size))
            (enemy + i)->place = tmp;
        if ((enemy + i)->place.x == enemy->place.x && (enemy + i)->place.y == enemy->place.y) {
            temp = "Attack " + (enemy + i)->name + " --> " + enemy->name + "\n";
            battleResult(enemy, (enemy + i));
            (enemy + i)->place.x = enemy->health <= 0 ? (enemy + i)->place.x : tmp.x;
            (enemy + i)->place.y = enemy->health <= 0 ? (enemy + i)->place.y : tmp.y;
        }
    }
    return temp;
}

//Валидация и выбор команды
int choosingOoption() {    
    std::vector <std::string> pattern{ "a","d","s","w","save", "exit"};
    std::cout << R"(Enter the command:
 a - move left
 d - move right
 s - move down 
 w - move up
     save
     exit     
)";
    std::string option;
    std::getline(std::cin, option);
    std::transform(option.begin(), option.end(), option.begin(), ::tolower);
    int pos{ -1 };
    for (int i = 0; i < pattern.size(); i++) {
        if (option == pattern[i])
            pos = i;
    }
    return pos;
}

//Загрузка игры
//Попробовал использовать библиотеку filesystem
bool loadGame(player* enemy, int& enemy_size)
{
    std::filesystem::path p_path = std::filesystem::current_path();

    std::filesystem::path p_path_dir = p_path / constants::DIR;
    std::filesystem::path p_path_file= p_path_dir / constants::FILE;
    std::filesystem::path p_dir = p_path.parent_path();
    bool ex_dir = std::filesystem::exists(p_path_dir);
    bool ex_file = std::filesystem::exists(p_path_file);

    if (!ex_dir)
    {
        std::cout << "Missing folder!\n" << p_path_dir;
        return false;
    }

    if (!ex_file) 
    {
        std::cout << "Missing file!\n" << p_path_file;
        return false;
    }

    std::ifstream ifs(p_path_file, std::ios::binary);

    if (ifs.fail())
    {
        std::cout << "File not found!";
    }
    if (!ifs.is_open())
    {
        std::cout << "File access error!";
        return true;
    }
    else
    {        
        int len_name{ 0 };
        ifs.read((char*)&enemy_size, sizeof(enemy_size));
        for (int i = 0; i < enemy_size; i++)
        {
            ifs.read((char*)&len_name, sizeof(len_name));            
            (enemy + i)->name.resize(len_name);
            ifs.read((char*)(enemy + i)->name.c_str(), len_name);
            ifs.read((char*)&(enemy + i)->health, sizeof((enemy + i)->health));
            ifs.read((char*)&(enemy + i)->armor, sizeof((enemy + i)->armor));
            ifs.read((char*)&(enemy + i)->damage, sizeof((enemy + i)->damage));
            ifs.read((char*)&(enemy + i)->place.x, sizeof((enemy + i)->place.x));
            ifs.read((char*)&(enemy + i)->place.y, sizeof((enemy + i)->place.y));
            
        }
    }
    ifs.close();
    return true;
}

//Сохранение игры
void saveGame(player* enemy, int& enemy_size)
{
    std::filesystem::path p_path = std::filesystem::current_path();    
    p_path /= constants::DIR;
    std::filesystem::create_directories(p_path);
    p_path /= constants::FILE;
    

    std::ofstream ofs(p_path, std::ios::binary);
    
    if (!ofs.is_open())
    {
        std::cout << "File access error!";
        return;
    }
    else
    {
        ofs.write((char*)&enemy_size, sizeof(enemy_size));
        for (int i = 0; i < enemy_size; i++)
        {
            int len_name = (enemy + i)->name.length();
            ofs.write((char*)&len_name, sizeof(len_name));
            ofs.write((char*)(enemy + i)->name.c_str(), len_name);
            ofs.write((char*)&(enemy + i)->health, sizeof((enemy + i)->health));
            ofs.write((char*)&(enemy + i)->armor, sizeof((enemy + i)->armor));
            ofs.write((char*)&(enemy + i)->damage, sizeof((enemy + i)->damage));
            ofs.write((char*)&(enemy + i)->place.x, sizeof((enemy + i)->place.x));
            ofs.write((char*)&(enemy + i)->place.y, sizeof((enemy + i)->place.y));
        }
    }
    ofs.close();
}

//Команды в мгре
void comand(player* enemy, int enemy_size)
{
    std::string tmp_player = "";
    std::string tmp_enemy = "";
    while (true) {
        int option = choosingOoption();
        while (option == -1) {
            std::cout << "Incorrect input, try again!\n";
            option = choosingOoption();
        }
        int flag{ 0 };
        switch (option) {
        case 0:
            std::cout << "0";
            tmp_player = playersMove(option, enemy, enemy_size);
            tmp_enemy = enemyMove(enemy, enemy_size);
            flag = resizePlayerArray(enemy, enemy_size);
            showResult(enemy, enemy_size, tmp_player, tmp_enemy, flag);
            if (flag != 0) return;
            break;
        case 1:
            std::cout << "1";
            tmp_player = playersMove(option, enemy, enemy_size);
            tmp_enemy = enemyMove(enemy, enemy_size).empty();
            flag = resizePlayerArray(enemy, enemy_size);
            showResult(enemy, enemy_size, tmp_player, tmp_enemy, flag);
            if (flag != 0) return;
            break;
        case 2:
            std::cout << "2";
            tmp_player = playersMove(option, enemy, enemy_size);
            tmp_enemy = enemyMove(enemy, enemy_size).empty();
            flag = resizePlayerArray(enemy, enemy_size);
            showResult(enemy, enemy_size, tmp_player, tmp_enemy, flag);
            if (flag != 0) return;
            break;
        case 3:
            std::cout << "3";
            tmp_player = playersMove(option, enemy, enemy_size);
            tmp_enemy = enemyMove(enemy, enemy_size).empty();
            flag = resizePlayerArray(enemy, enemy_size);
            showResult(enemy, enemy_size, tmp_player, tmp_enemy, flag);
            if (flag != 0) return;
            break;
        case 4:
            std::cout << "4";
            saveGame(enemy, enemy_size);
            break;
        case 5:
            return;
        }
    }
}

//Новая игра
void startGameNew() {
    bool playing_field[constants::MAX_MAP][constants::MAX_MAP]{};
    player enemy[constants::COUNT_PLAYER]{};
    int enemy_size = constants::COUNT_PLAYER;    
    creatHero(enemy);
    creatEnemy(*playing_field, constants::MAX_MAP, enemy, constants::COUNT_PLAYER);    
    showMap(enemy, enemy_size);    

    comand(enemy, enemy_size);
}

//Игра из сохранения
void startGameLoad() {
    int enemy_size = constants::COUNT_PLAYER;
    player enemy[constants::COUNT_PLAYER]; 
    std::string tmp_player = "";
    std::string tmp_enemy = "";
    
    bool err = loadGame(enemy, enemy_size);
    if (!err)
    {
        std::cout << "\nThere is no saved game!\nA new game!\n";
        startGameNew();
        return;
    }
        
    showResult(enemy, enemy_size, tmp_player, tmp_enemy);
    comand(enemy, enemy_size);
}

void startGame() {
    std::vector <std::string> pattern{ "1","2" };    
    std::string option;        
    int pos{ -1 };

    do
    {
        std::cout << R"(Enter the command:
 1 - New game
 2 - Download a saved game 
)";
        std::getline(std::cin, option);
        for (int i = 0; i < pattern.size(); i++) {
            if (option == pattern[i])
                pos = i;
        }
        if (pos != -1)
            break;
        std::cout << "Incorrect input, try again!\n";

    } while (true);
    
    switch (pos)
    {
    case 0:
        startGameNew();
        break;
    case 1:
        startGameLoad();
        break;        
    }
}

int main()
{
    std::srand(std::time(NULL));

    startGame();     

}

