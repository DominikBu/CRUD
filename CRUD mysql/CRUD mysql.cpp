#include <iostream>
#include <mysql.h>    //Biblioteka umożliwiająca nam kożsytanie z baz danych
#include <windows.h>
#include <string>

using namespace std;

//Funkcja zmieniająca tabele na której bedziemy operować
void zmiana_tabeli(int& tab, string& tabel)
{
    do
    {
        //Wybor tabeli z naszej bazy danych (w tym przypadku z bazy danych world)
        cout << "Podaj na jakiej tabeli chcesz operowac" << endl;
        cout << "[1] kierowcy" << endl;
        cout << "[2] pojazdy" << endl;
        cout << "[3] trasy" << endl;

        cin >> tab;
        system("cls");
        switch (tab)
        {
        case 1:
            tabel = "kierowcy";
            break;
        case 2:
            tabel = "pojazdy";
            break;
        case 3:
            tabel = "trasy";
            break;
        default:
            cout << "Niepoprwny wybor!" << endl;
        }

        cout << endl;
    } while (!(tab == 1 || tab == 2 || tab == 3));
    system("cls");
}


//Funkcja wyświetlająca tabele
void Wyswietl( MYSQL * mysql, string tabela)
{
    MYSQL_ROW row;              //Zmienne przechowywująca wiersz tabeli
    MYSQL_RES* res;             //Zmienna przechowywująca resultat polecenia
    MYSQL_FIELD* field;         //Zmienna przechowywująca nazwy kolumn

    //Odczytywanie wszystkich danych z tabeli
    string tabel = "SELECT * FROM " + tabela;
    const char* t = tabel.c_str();
    int qstate = mysql_query(mysql, t);

    
    //Udalo sie wykonac polecenie
    if (!qstate)
    {

        res = mysql_store_result(mysql);    //Zapisanie wyniku do zmiennej
        int count = mysql_num_fields(res);  //Zapisanie ilosci kolumn

        //Wypisanie nazw kolumn
        while (field = mysql_fetch_field(res))
        {
            cout << field->name << "\t";
        }
        cout << endl << endl;

        //Wypisanie wszystkich wierszy, w przypadku braku wartości pola wypisanie NULL
        while ((row = mysql_fetch_row(res)) != NULL)
        {
            for (int i = 0; i < count; i++)
            {
                if (row[i] == NULL)
                    cout << "NULL\t";
                else
                    cout << row[i] << "\t";
            }
            cout << endl << endl;
        }
    }
    else
    {
        cout << "Nie udalo sie wyswietlic tabeli!" << endl;
    }
    cout << endl;
}

//Funkcja dodająca nowy wiersz do tabeli
void Dodaj(MYSQL* mysql, string tabela)
{
    MYSQL_ROW row;                  //Zmienne przechowywująca wiersz tabeli
    MYSQL_RES* res;                 //Zmienna przechowywująca resultat polecenia
    MYSQL_FIELD* field;             //Zmienna przechowywująca nazwy kolumn

    //Odczytywanie wszystkich danych z tabeli
    string tabel = "SELECT * FROM " + tabela;
    const char* t = tabel.c_str();
    int qstate = mysql_query(mysql, t);

    //Udalo sie wykonac polecenie
    if (!qstate)
    {

        res = mysql_store_result(mysql);        //Zapisanie wyniku do zmiennej
        int count = mysql_num_fields(res);      //Zapisanie ilosci kolumn

        //Allokacja pamieci do zapisania nazw kolumn
        string* columns = new string[count];

        //zmienne przechowywująca zapytanie
        string query = "INSERT INTO `" + tabela + "` VALUES( ";

        //Podanie wartości pól
        for (int i = 0; i < count; i++)
        {
            field = mysql_fetch_field(res);
            cout << "Podaj wartosc pola: " << field->name << endl;
            if(i==0)
                getline(cin, columns[i]);
            getline(cin, columns[i]);
            if (i < (count -  1))
                query = query + "'" + columns[i] + "', ";
            else
                query = query + "'" + columns[i] + "'";
        }
        query = query + " )";
        const char* q = query.c_str();
        qstate = mysql_query(mysql, q);         //Wysłanie zbudowanego polecenia na serwer
        
        if (!qstate)
        {
            cout << "Udalo sie dodac element!" << endl;
        }
        else
        {
            cout << "Nie udalo sie dodac elementu!" << endl;
        }

        delete[] columns;
    }
    else
    {
        cout << "Nie udalo sie znalezc tabeli" << endl;
    }

    cout << endl;
}

//Funkcja zapisuje wszystkie dane z tabeli (umiejętność odczytywania danych) ale wypisuje tylko tą o którą poprosimy
void Odczytaj(MYSQL* mysql, string tabela)
{
    MYSQL_ROW row;
    MYSQL_RES* res;
    MYSQL_FIELD* field;

    string tabel = "SELECT * FROM " + tabela;
    const char* t = tabel.c_str();
    int qstate = mysql_query(mysql, t);

    if (!qstate)
    {
        res = mysql_store_result(mysql);
        int countf = mysql_num_fields(res);     //Ilość kolumn
        int countr = mysql_num_rows(res);       //Ilość wierszy

        //Allokacja pamieći w celu zapisania nazw kolumn i wszystkich wartości wierszy
        string* columns = new string[countf];
        string** spot = new string * [countf];
        for (int i = 0; i < countf; i++)
            spot[i] = new string[countr];

        //Zapisanie wszystkich wartości do tablicy dwuwymiarowej gdzie pierwszy argument to kolumna, a drugi to wiersz
        for(int i=0; i<countr; i++)
        {
            row = mysql_fetch_row(res);

            for (int j = 0; j < countf; j++)
            {
                if (row[j] == NULL)
                    spot[j][i] = "NULL";
                else
                    spot[j][i] = row[j];
            }
        }

        //Zapisannie nazw kolumn
        for (int i = 0; i < countf; i++)
        {
            field = mysql_fetch_field(res);
            columns[i] = field->name;
        }

        cout << "Podaj kolumne oraz wiersz dla ktorej chcesz uzyskac dane" << endl;
        cout << "Ilosc kolumn: " << countf << endl;
        cout << "Ilosc wierszy: " << countr << endl;
        int w, k;
        cin >> k >> w;
        
        if (k > countf || k <= 0 || w > countr || w <= 0)
        {
            cout << "Podano bledne dane" << endl;
            return;
        }

        cout << "Kolumna: " << columns[k-1] << endl << "Wartosc: " << spot[k-1][w-1] << endl;

        for (int i = 0; i < countf; i++)
            delete[] spot[i];
        delete[] spot;
        delete[] columns;
    }
    else
    {
        cout << "Nie udalo sie znalezc tabeli" << endl;
    }

    cout << endl;
}

//Edycja wartości w tabeli
void Edytuj(MYSQL* mysql, string tabela)
{
    MYSQL_ROW row;
    MYSQL_RES* res;
    MYSQL_FIELD* field;

    string tabel = "SELECT * FROM " + tabela;
    const char* t = tabel.c_str();
    int qstate = mysql_query(mysql, t);

    if (!qstate)
    {
        res = mysql_store_result(mysql);
        int countf = mysql_num_fields(res);
        int countr = mysql_num_rows(res);

        string* columns = new string[countf];
        string** spot = new string * [countf];
        for (int i = 0; i < countf; i++)
            spot[i] = new string[countr];

        for (int i = 0; i < countr; i++)
        {
            row = mysql_fetch_row(res);

            for (int j = 0; j < countf; j++)
            {
                if (row[j] == NULL)
                    spot[j][i] = "NULL";
                else
                    spot[j][i] = row[j];
            }
        }

        for (int i = 0; i < countf; i++)
        {
            field = mysql_fetch_field(res);
            columns[i] = field->name;
        }

        cout << "Podaj kolumne oraz wiersz dla ktorej chcesz uzyskac dane i je zmienic: " << endl;
        cout << "Ilosc kolumn: " << countf << endl;
        cout << "Ilosc wierszy: " << countr << endl;
        int w, k;
        cin >> k >> w;

        if (k > countf || k <= 0 || w > countr || w <= 0)
        {
            cout << "Podano bledne dane" << endl;
            return;
        }

        cout << "Kolumna: " << columns[k - 1] << endl << "Wartosc: " << spot[k - 1][w - 1] << endl;

        cout << "Podaj wartosc na jaka chcesz podmienic:" << endl;

        //Zmienna przechowywująca zapytanie UPDATE
        string wartosc;
        getline(cin, wartosc);
        getline(cin, wartosc);
        wartosc = "UPDATE `" + tabela + "` SET " + columns[k - 1] + "='" + wartosc + "' WHERE ";

        //Budowanie zapytania jako stringa aby napewno trafic w odpowiedni rekord, wartości NULL są będą nieuwzględniane w klauzuli WHERE
        for (int i = 0; i < countf; i++)
        {
            if (i == k - 1)
                continue;
            if (spot[i][w - 1] == "NULL")
                continue;
            wartosc = wartosc + columns[i] + "='" + spot[i][w-1] + "' AND ";
        }
        wartosc = wartosc + columns[k - 1] + "=" + columns[k - 1];

        //Wysłanie zapytania
        const char* q = wartosc.c_str();
        qstate = mysql_query(mysql, q);

        if (!qstate)
        {
            cout << "Udalo sie edytowac element!" << endl;
        }
        else
        {
            cout << "Nie udalo sie edytowac elementu!" << endl;
        }

        for (int i = 0; i < countf; i++)
            delete[] spot[i];
        delete[] spot;
        delete[] columns;
    }
    else
    {
        cout << "Nie udalo sie znalezc tabeli" << endl;
    }
    cout << endl;
}

//Funkcja usuwająca wiersz tabeli
void Usun(MYSQL* mysql, string tabela)
{
    MYSQL_ROW row;
    MYSQL_RES* res;
    MYSQL_FIELD* field;

    string tabel = "SELECT * FROM " + tabela;
    const char* t = tabel.c_str();
    int qstate = mysql_query(mysql, t);

    if (!qstate)
    {
        res = mysql_store_result(mysql);
        int countf = mysql_num_fields(res);
        int countr = mysql_num_rows(res);

        string* columns = new string[countf];
        string** spot = new string * [countf];
        for (int i = 0; i < countf; i++)
            spot[i] = new string[countr];

        for (int i = 0; i < countr; i++)
        {
            row = mysql_fetch_row(res);

            for (int j = 0; j < countf; j++)
            {
                if (row[j] == NULL)
                    spot[j][i] = "NULL";
                else
                    spot[j][i] = row[j];
            }
        }

        for (int i = 0; i < countf; i++)
        {
            field = mysql_fetch_field(res);
            columns[i] = field->name;
        }

        cout << "Podaj wiersz ktory chcesz usunac: " << endl;
        cout << "Ilosc wierszy: " << countr << endl;
        int w;
        cin >> w;

        if ( w > countr || w <= 0)
        {
            cout << "Podano bledne dane" << endl;
            return;
        }

        for(int i=0; i<countf; i++)
        {
            cout << columns[i] << "\t";
        }
        cout << endl << endl;

        for (int i = 0; i < countf; i++)
        {
            cout << spot[i][w-1] << "\t";
        }
        cout << endl << endl;

        string wartosc;
        wartosc = "DELETE FROM `" + tabela + "` WHERE ";
        for (int i = 0; i < countf; i++)
        {
            if (spot[i][w - 1] == "NULL")
                continue;
            wartosc = wartosc + columns[i] + "='" + spot[i][w - 1] + "' AND ";
        }
        wartosc = wartosc + columns[0] + "=" + columns[0];

        const char* q = wartosc.c_str();
        qstate = mysql_query(mysql, q);

        if (!qstate)
        {
            cout << "Udalo sie usunac element!" << endl;
        }
        else
        {
            cout << "Nie udalo sie usunac elementu!" << endl;
        }

        for (int i = 0; i < countf; i++)
            delete[] spot[i];
        delete[] spot;
        delete[] columns;
    }
    else
    {
        cout << "Nie udalo sie znalezc tabeli" << endl;
    }
    cout << endl;
}

//Funkcja główna programu
int main(int argc, const char* argv[])
{
    //Tworzenie zmiennej odpowiedzialnej za połączenie z bazą danych
    MYSQL* mysql;
    
    //Łączenie z programem
    mysql = mysql_init(0);
    mysql = mysql_real_connect(mysql, "127.0.0.1", "root", "1234", "firma_transportowa", 0, NULL, 0);

    //W przypadku gdy się udało połączyć
    if (mysql)
    {
        //Zaczecie transakcji
        int qstate = mysql_query(mysql, "START TRANSACTION");
        int x = 1;
        int tab;
        string tabel;
        cout << "Polaczono!" << endl << endl;
        zmiana_tabeli(tab, tabel);

        //Menu Główne
        while (x)
        {
            cout << "\t\tFirma transportowa\t\t" << endl;
            cout << "Aktualna tabela: " << tabel << endl;
            cout << "Wybierz opcje:" << endl;
            cout << "[1] Wyswietl tabele" << endl;
            cout << "[2] Dodaj" << endl;
            cout << "[3] Odczytaj" << endl;
            cout << "[4] Edytuj" << endl;
            cout << "[5] Usun" << endl;
            cout << "[6] Zmien tabele" << endl;
            cout << "[7] Zatwierdz zmiany" << endl;
            cout << "[8] Anuluj zmiany" << endl;
            cout << "[0] Wyjdz" << endl;
    
            cin >> x;

            switch (x)
            {
            case 1:
                Wyswietl(mysql, tabel);
                break;
            case 2:
                Dodaj(mysql, tabel);
                break;
            case 3:
                Odczytaj(mysql, tabel);
                break;
            case 4:
                mysql_query(mysql, "SET FOREIGN_KEY_CHECKS=0;");
                Edytuj(mysql, tabel);
                mysql_query(mysql, "SET FOREIGN_KEY_CHECKS=1;");
                break;
            case 5:
                mysql_query(mysql, "SET FOREIGN_KEY_CHECKS=0;");
                Usun(mysql, tabel);
                mysql_query(mysql, "SET FOREIGN_KEY_CHECKS=1;");
                break;
            case 6:
                //Zmiana nazwy tabeli
                zmiana_tabeli(tab, tabel);
                break;
            case 7:
                //COMMIT aktualnych zmiann i rozpoczęcie nowej transakcji
                qstate = mysql_query(mysql, "COMMIT");
                if (!qstate)
                    cout << "Zatwierdzono zmiany" << endl << endl;
                else
                    cout << "Nie zatwierdzono zmiany" << endl << endl;
                mysql_query(mysql, "START TRANSACTION");
                break;
            case 8:
                //ROLLBACK aktualnych zmiann i rozpoczęcie nowej transakcji
                qstate = mysql_query(mysql, "ROLLBACK");
                if (!qstate)
                    cout << "Anulowano zmiany" << endl << endl;
                else
                    cout << "Blad przy anulowaniu" << endl << endl;
                mysql_query(mysql, "START TRANSACTION");
                break;
            case 0:
                x = 0;
                break;
            }
        }
    }
    else
        cout << "Nie polaczono" << endl << mysql_errno(mysql) << mysql_error(mysql) << endl;;



    mysql_close(mysql); // zamknij połączenie

    return 0;
}