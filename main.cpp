#include <cmath>
#include <vector>
#include <fstream>
#include <array>
#include <iostream>

using namespace std;

float vectorLenght(std::array <float, 2> vector1)                                //funkcja obliczajaca dlugosc wektora [x,y]
{
    return sqrt(pow(vector1[0],2)+pow(vector1[1],2));
}

float dotProduct(std::array <float, 2> vector1, std::array <float, 2> vector2)   //funkcja obliczajaca iloczyn skalarny dwoch wektorow
{
    return ((vector1[0]*vector2[0])+(vector1[1]*vector2[1]));
}

float vectorAngle(std::array <float, 2> vector1, std::array <float, 2> vector2)  //funkcja obliczajaca kat miedzy dwoma wektorami
{
    return ((180.0 / M_PI)*acos(dotProduct(vector1, vector2)/(vectorLenght(vector1)*vectorLenght(vector2))));
}

int main()
{
    ifstream en("enter.txt");                                               //wejscie: liczba punktow, a nastepnie wspolrzedne X i Y kolejnych punktow
    ofstream out("out.txt");                                                //wyjscie: punkty bedace czescia otoczki, podane w kolejnosci przeciwnej do ruchu wskazowek zegara, poczawszy od punktu znajdujacego sie najnizej i najbardziej w lewo
    int pointCount;
    bool hullClosed = false;                                                //ta zmienna przyjmie wartosc true kiedy otoczka sie zamknie
    en>>pointCount;                                                         //program przyjmuje liczbe punktow
    float points[pointCount][3];                                            //tworzymy tablice ktora przechowa punkty, oraz trzecia wartosc 0/1 ktora powie nam, czy dany punkt jest juz czescia otoczki. Ta trzecia wartosc nazwiemy zmienna kontrolna
    for (int i=0; i<pointCount; i++)                                        //wczytujemy punkty do tablicy
    {
        en>>points[i][0];
        en>>points[i][1];
        points[i][2] == 0;                                                  //z poczatku nie zakladamy ze ktorykolwiek punkt na pewno jest czescia otoczki
    }
    int minY = points[0][1];                                                           //szukamy indeksu punktu o najnizszej wspolrzednej Y, a jesli jest takich kilka to wybieramy ten z nich ktory ma najmniejsza wspolrzedna X. Ten punkt na pewno bedzie czescia otoczki, ale nie zmieniamy jego zmiennej kontrolnej aby moc wykryc kiedy otoczka sie zamknie
    for (int i=1;i<pointCount;i++)
    {
        if (points[minY][1]>points[i][1])
            minY = i;
        else if (points[minY][1]==points[i][1])
                if (points[minY][0]>points[i][0])
                    minY = i;
    }
                                                                            //mimo tego ze ten punkt jest na pewno czescia otoczki, nie zmieniamy wartosci jego zmiennej kontrolnej. robimy to, aby moc dalej brac go pod uwage w porownaniach i wykryc moment zamkniecia otoczki
    vector<std::array<float,2>> hull;                                       //tworzymy tablice dynamiczna do ktorej bedziemy zapisywac wektory bedace czescia otoczki
    array <float, 2> firstPoint = {points[minY][0], points[minY][1]};       //przechowujemy pierwszy punkt w osobnej tablicy, ktorej uzyjemy pozniej do sprawdzania czy otoczka sie zamknela. Jako ze pierwszy punkt jest jednoczesnie ostatnim jaki w tym momencie znamy, zostanie tez chwilowo zapisany w tablicy lastPoint
    array <float, 2> lastPoint ={points[minY][0], points[minY][1]};         //wedle tego wzoru bedziemy tworzyc kolejne wiersze, ktore nastepnie dopisywac bedziemy do tablicy. Tablice nazywamy 'lastPoint' bo bedziemy w niej przechowywac ostatni znany punkt bedacy czescia otoczki
    hull.push_back(lastPoint);                                              //dodajemy pierwszy punkt do tablicy 'hull'

    array <float, 2> vector1 = {-1,0};                                      //vector1 to wektor przebigajacy od ostatnio odkrytego punktu do przedostatniego odkrytego punktu
                                                                            //jako ze w tym momencie znamy tylko jeden punkt, deklarujemy vector1 jako maly fragment rownolegly do osi OX zaczepiony w pierwszym znanym nam punkcie
    int candidate;                                                          // w tej zmiennej bedziemy przechowywac indeks punktu 'kandydujacego' do bycia wlaczonym do otoczki (takiego ktory w danym momencie tworzylby najwiekszy kat z ostatnim znanym bokiem)
    float candidateVectorX, candidateVectorY;                               //tu bedziemy przechowywac wspolrzedne wektora miedzy ostatnim znanym punktem, a punktem kandydujacym chwilowo do wlaczenia do otoczki
    while (!hullClosed)                                                     //petla while znajduje kolejne elementy otoczki az do momentu, kiedy sie ona domknie
    {
        float maxAngle = 0;                                                 //do otoczki dolaczymy taki punkt, aby utworzony na nim nowy bok mial najwiekszy mozliwy kat z bokiem poprzednim.
        for (int i=0;i<pointCount;i++)
        {
            if (points[i][2]==1)                                            //jezeli juz wiemy ze dany punkt jest czescia otoczki, to go nie rozpatrujemy
                continue;
            array <float, 2> vector2 = {points[i][0]-lastPoint[0], points[i][1]-lastPoint[1]};   //vector2 laczy ostatni znany punkt z punktem chwilowo rozpatrywanym
            if (vectorAngle(vector1, vector2) >= maxAngle)                   //jesli bok utworzony z rozpatrywanym obecnie punktem tworzyl najwiekszy kat z poprzednim bokiem, zostaje on kandydatem do wlaczenia do otoczki
            {
                if (abs(lastPoint[0] - points[i][0]) < abs(lastPoint[0]) - points[candidate][0]) //musimy rozpatrzec edge-case w ktorym dwa kandydujace punkty sa wspolliniowe wzgledem punktu wlaczonego juz do otoczki. Jako chwilowego kandydata wybieramy wowczas punkt znajdujacy sie blizej punktu wlaczonego juz do otoczki.
                {
                    maxAngle = vectorAngle(vector1, vector2);                   //ustawiamy wartosc z ktora od teraz bedziemy porownywac kolejne katy
                    candidate = i;                                              //zmieniamy indeks kandydata
                    candidateVectorX = lastPoint[0]-points[i][0];               //i zapisujemy wspolrzedne wektora prowadzacego od ostatniego punktu do kandydata
                    candidateVectorY = lastPoint[1]-points[i][1];
                }
                else
                {
                    maxAngle = vectorAngle(vector1, vector2);                   //ustawiamy wartosc z ktora od teraz bedziemy porownywac kolejne katy
                    candidate = i;                                              //zmieniamy indeks kandydata
                    candidateVectorX = lastPoint[0]-points[i][0];               //i zapisujemy wspolrzedne wektora prowadzacego od ostatniego punktu do kandydata
                    candidateVectorY = lastPoint[1]-points[i][1];
                }
            }
        }
        lastPoint[0] = points[candidate][0];
        lastPoint[1] = points[candidate][1];                                //po znalezieniu odpowiedniego punktu, wczytujemy go do wiersza ktory nastepnie dolaczymy do tabeli 'hull'
        if (lastPoint[0] == firstPoint[0] && lastPoint[1] == firstPoint[1]) //sprawdzamy czy otoczka sie zamknela
            hullClosed = true;                                              //jesli tak, nie dopisujemy tego punktu (byl dopisany na poczatku, przed petla while) i zamykamy petle
        else
            {
                points[candidate][2] = 1;                                   //zmieniamy zmienna kontrolna tak, aby funkcja nie brala juz tego punktu pod uwage
                hull.push_back(lastPoint);                                  //dodajemy punkt do tablicy 'hull'
                vector1[0] = candidateVectorX;
                vector1[1] = candidateVectorY;
            }
    }
    for(array<float, 2> i : hull)                                           //petla wyprowadza nam wszystkie elementy otoczki
        out<<i[0] <<" " <<i[1] <<endl;
    return 0;
}

