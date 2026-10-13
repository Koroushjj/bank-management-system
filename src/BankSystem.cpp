#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;
string aktuellesDatum() {
    auto now = chrono::system_clock::now();
    time_t zeit = chrono::system_clock::to_time_t(now);

    tm localTime;
    localtime_s(&localTime, &zeit);

    stringstream ss;
    ss << put_time(&localTime, "%d.%m.%Y");
    return ss.str();
}

class Transaktion {
private:
	string typ;
	double betrag;
	string datum;
	string beschreibung;
public:
	Transaktion(string t, double b, string d, string be)
		: typ(t), betrag(b), datum(d), beschreibung(be) {
	}
	void getInfo() {
		cout << "Typ: " << typ << endl;
		cout << "Betrag: " << betrag << endl;
		cout << "Datum: " << datum << endl;
		cout << "Beschreibung: " << beschreibung << endl;
	}
};

class Konto {
private:
	int kontonummer;
	double kontostand;
	bool gesperrt;
	static int naechsteKontonummer;
    string kontoArt;
    string inhaber;
	vector<Transaktion> historie;
public:
	Konto(double stand, string name,string art)
		: kontostand(stand), gesperrt(false), inhaber(name), kontoArt(art) {
		kontonummer = naechsteKontonummer;
		naechsteKontonummer++;
	}
	bool einzahlen(double Betrag) {
		if (gesperrt) {
			cout << "Konto ist gesperrt!" << endl;
			return false;
		}
        if (Betrag <= 0) {
            cout << "Betrag muss groesser als 0 sein!" << endl;
            return false;
        }
		kontostand += Betrag;
		historie.push_back(
			Transaktion("Einzahlung", Betrag, aktuellesDatum(), "Geldeinzahlung")
		);
        return true;
	}

    virtual bool auszahlen(double Betrag) {
		if (gesperrt) {
			cout << "Konto ist gesperrt!" << endl;
            return false;
		}
		if (Betrag > kontostand) {
			cout << "Nicht genug Guthaben!" << endl;
            return false;
		}
        if (Betrag <= 0) {
            cout << "Betrag muss groesser als 0 sein!" << endl;
            return false;
        }
		kontostand -= Betrag;
		historie.push_back(
			Transaktion("Auszahlung", Betrag,aktuellesDatum(), "Geldauszahlung")
		);
        return true;
        
	}
	void zeigeHistorie() {
		if (historie.empty()) {
			cout << "Keine Transaktionen vorhanden." << endl;
			return;
		}

		for (int i = 0; i < historie.size(); i++) {
			cout << "\n<<Transaktion>> " << endl;
			historie[i].getInfo();
		}
	}
	double getKontostand() {
		return kontostand;
	}	
	int getKontonummer() {
		return kontonummer;
	}
    string getInhaber() {
        return inhaber;
    }
    string getKontoArt() {
        return kontoArt;
    }
	void sperren() {
		gesperrt = true;
	}

	void entsperren() {
		gesperrt = false;
	}
    void transaktionHinzufuegen(string typ, double betrag, string beschreibung) {
        historie.push_back(
            Transaktion(typ, betrag, aktuellesDatum(), beschreibung)
        );
    }
    void kontostandAendern(double betrag) {
        kontostand += betrag;
    }
    bool istGesperrt() {
        return gesperrt;
    }
    virtual bool ueberweisungErlaubt() {
        return true;
    }
};

class Benutzer {
private:
	int BenutzerID;
	string Name;
	vector<Konto*> konten;
	static int naechsteID;   
public :
	Benutzer(string n) 
		: Name(n) {
		BenutzerID = naechsteID;
		naechsteID++; 
	}
	void kontoHinzufuegen(Konto* konto) {
		konten.push_back(konto);
	}
    Konto* MykontoSuchen(int nummer) {
        for (Konto* konto : konten) {
            if (konto->getKontonummer() == nummer) {
                return konto;
            }
        }
        return nullptr;
    }
    vector<Konto*>& getKonten() {
        return konten;
    }

    string getName() {
        return Name;
    }

    int getBenutzerID() {
        return BenutzerID;
    }
};

class Girokonto : public Konto {
private:
    double ueberziehungsLimit = 100;

public:
    Girokonto(double stand, string name)
        : Konto(stand, name, "Girokonto") {
    }

    bool auszahlen(double Betrag) override {
        if (istGesperrt()) {
            cout << "Konto ist gesperrt!" << endl;
            return false;
        }

        if (getKontostand() - Betrag < -ueberziehungsLimit) {
            cout << "Ueberziehungslimit ueberschritten!" << endl;
            return false;
        }

        kontostandAendern(-Betrag);

        transaktionHinzufuegen(
            "Auszahlung",
            Betrag,
            "Girokonto-Auszahlung"
        );

        cout << "Aktueller Kontostand: " << getKontostand() << " Euro" << endl;

        return true;
    }
};

class Sparkonto : public Konto {
public:
    Sparkonto(double stand, string name)
        : Konto(stand, name, "Sparkonto") {
    }
    bool ueberweisungGesperrt = true;
    bool ueberweisungErlaubt() override {
        return !ueberweisungGesperrt;
    }
    void ueberweisungFreigeben() {
        ueberweisungGesperrt = false;
    }

    void ueberweisungSperren() {
        ueberweisungGesperrt = true;
    }
};

class Bank {
private:
	vector<Benutzer*> benutzerliste;
	vector<Konto*> KontoListen;
	vector<Transaktion> bankHistorie;
public:
	void BenutzerHinzufuegen(Benutzer* benutzer) {
		benutzerliste.push_back(benutzer);
		cout << benutzer->getName() << " wurde hinzugefuegt." << endl;
	}
	void KontoHinzufuegen(Konto* Konto) {
		KontoListen.push_back(Konto);
	}
	Konto* KontoSuchen(int Nummer) {
		for (Konto* Allkonto : KontoListen){
			if (Allkonto->getKontonummer() ==	Nummer) {
				return Allkonto;
			}
		}
		return nullptr;
	}
	void ueberweisen(int von, int nach, double betrag) {
		Konto* sender = KontoSuchen(von);
		Konto* empfaenger = KontoSuchen(nach);

		if (sender == nullptr || empfaenger == nullptr) {
			cout << "Konto nicht gefunden!" << endl;
			return;
		}
        if (sender->istGesperrt()) {
            cout << "Senderkonto ist gesperrt!" << endl;
            return;
        }
        if (empfaenger->istGesperrt()) {
            cout << "Empfaengerkonto ist gesperrt!" << endl;
            return;
        }
		if (sender->getKontostand() < betrag) {
			cout << "Nicht genug Guthaben!" << endl;
			return;
		}
        sender->kontostandAendern(-betrag);
        empfaenger->kontostandAendern(betrag);

        sender->transaktionHinzufuegen(
            "Ueberweisung - Ausgang",
            betrag,
            "Betrag wurde an Konto " + to_string(nach) + " ueberwiesen"
        );

        empfaenger->transaktionHinzufuegen(
            "Ueberweisung - Eingang",
            betrag,
            "Betrag wurde von Konto " + to_string(von) + " erhalten"
        );

		cout << "Ueberweisung erfolgreich!" << endl;
		cout << "Ueberweisungsbetrag: : "<< betrag << endl;
		cout << "Von Konto : " << von << " Nach Konto :" << nach << endl;
        bankHistorie.push_back(
            Transaktion(
                "Ueberweisung",
                betrag,
                aktuellesDatum(),
                "Betrag wurde von Konto " +
                to_string(von) +
                " an Konto " +
                to_string(nach) +
                " ueberwiesen"
            )
        );
	}
    void alleKontenAnzeigen() {
        if (KontoListen.empty()) {
            cout << "Keine Konten vorhanden." << endl;
            return;
        }

        for (int i = 0; i < KontoListen.size(); i++) {
            cout << "\nKonto " << i + 1 << endl;
            cout << "Inhaber: " << KontoListen[i]->getInhaber() << endl;
            cout << "Kontonummer: " << KontoListen[i]->getKontonummer() << endl;
            cout << "Kontoart: " << KontoListen[i]->getKontoArt() << endl;
            cout << "Kontostand: " << KontoListen[i]->getKontostand() << " Euro" << endl;
        }
    }
    void transaktionSpeichern(string typ, double betrag, string beschreibung) {
        bankHistorie.push_back(
            Transaktion(typ, betrag, aktuellesDatum(), beschreibung)
        );
    }
    void zeigeBankHistorie() {
        if (bankHistorie.empty()) {
            cout << "Keine Transaktionen vorhanden." << endl;
            return;
        }

        for (int i = 0; i < bankHistorie.size(); i++) {
            cout << "\n<<Bank-Transaktion>>" << endl;
            bankHistorie[i].getInfo();
        }
    }

};

int leseInt(string text) {
    int wert;

    while (true) {
        cout << text;
        cin >> wert;

        if (!cin.fail()) {
            return wert;
        }

        cout << "Ungueltige Eingabe! Bitte eine ganze Zahl eingeben." << endl;

        cin.clear();
        cin.ignore(1000, '\n');
    }
}

double leseDouble(string text) {
    double wert;

    while (true) {
        cout << text;
        cin >> wert;

        if (!cin.fail()) {
            return wert;
        }

        cout << "Ungueltige Eingabe! Bitte eine Zahl eingeben." << endl;

        cin.clear();
        cin.ignore(1000, '\n');
    }
}
int Konto::naechsteKontonummer = 1001;
int Benutzer::naechsteID = 100;
int main() {
    Bank b;
    Benutzer* b1 = nullptr;

    int startAuswahl;
    int auswahl;
    string name;
    double startBetrag;

    while (true) {
        cout << "\n===== WHZ BANKENSYSTEM =====" << endl;
        cout << "1. Benutzer" << endl;
        cout << "2. Bank-Administration" << endl;
        cout << "3. Beenden" << endl;
        startAuswahl = leseInt("Auswahl: ");

        switch (startAuswahl) {

        case 1: {
            if (b1 == nullptr) {
                cout << "Kein Konto vorhanden. Bitte erstellen Sie zuerst ein Konto." << endl;
                cout << "Geben Sie Ihren Namen ein: ";
                cin >> name;

                b1 = new Benutzer(name);
                b.BenutzerHinzufuegen(b1);

                cout << "Bitte erstellen Sie zuerst ein Konto." << endl;
                cout << "<<Kontotyp auswaehlen:>>" << endl;
                cout << "1. Standardkonto" << endl;
                cout << "2. Girokonto" << endl;
                cout << "3. Sparkonto" << endl;
                
                int kontoTyp;
                while (true) {
                    kontoTyp = leseInt("Kontoart waehlen: ");

                    if (kontoTyp >= 1 && kontoTyp <= 3) {
                        break;
                    }

                    cout << "Ungueltige Kontoart. Bitte 1, 2 oder 3 eingeben." << endl;
                }

                startBetrag = leseDouble("Geben Sie den Startbetrag ein: ");

                Konto* k1 = nullptr;

                if (kontoTyp == 1) {
                    k1 = new Konto(startBetrag, b1->getName(), "Standardkonto");
                }
                else if (kontoTyp == 2) {
                    k1 = new Girokonto(startBetrag, b1->getName());
                }
                else if (kontoTyp == 3) {
                    k1 = new Sparkonto(startBetrag, b1->getName());
                }

                b1->kontoHinzufuegen(k1);
                b.KontoHinzufuegen(k1);

                cout << "Konto wurde erfolgreich erstellt." << endl;
                cout << "Benutzer-ID: " << b1->getBenutzerID() << endl;
                cout << "Name: " << b1->getName() << endl;
                cout << "Kontonummer: " << k1->getKontonummer() << endl;
                cout << "Kontoart: " << k1->getKontoArt() << endl;
                cout << "Kontostand: " << k1->getKontostand() << " Euro" << endl;

            }

            while (true) {
                cout << "\n--- Benutzer-Menue ---" << endl;
                cout << "1. Konto erstellen" << endl;
                cout << "2. Einzahlen" << endl;
                cout << "3. Auszahlen" << endl;
                cout << "4. Kontostand anzeigen" << endl;
                cout << "5. Transaktionshistorie anzeigen" << endl;
                cout << "6. Ueberweisen" << endl;
                cout << "7. Zurueck" << endl;
                auswahl = leseInt("Auswahl: ");

                switch (auswahl) {

                case 1: {
                    cout << "<<Kontotyp auswaehlen:>>" << endl;
                    cout << "1. Standardkonto" << endl;
                    cout << "2. Girokonto" << endl;
                    cout << "3. Sparkonto" << endl;
                    int kontoTyp;

                    while (true) {
                        kontoTyp = leseInt("Kontoart waehlen: ");

                        if (kontoTyp >= 1 && kontoTyp <= 3) {
                            break;
                        }

                        cout << "Ungueltige Kontoart. Bitte 1, 2 oder 3 eingeben." << endl;
                    }

                    startBetrag = leseDouble("Geben Sie den Startbetrag ein: ");

                    Konto* k1 = nullptr;

                    if (kontoTyp == 1) {
                        k1 = new Konto(startBetrag, b1->getName(), "Standardkonto");
                    }
                    else if (kontoTyp == 2) {
                        k1 = new Girokonto(startBetrag, b1->getName());
                    }
                    else if (kontoTyp == 3) {
                        k1 = new Sparkonto(startBetrag, b1->getName());
                    }

                  
                    b1->kontoHinzufuegen(k1);
                    b.KontoHinzufuegen(k1);

                    cout << "Konto wurde erfolgreich erstellt." << endl;
                    cout << "Name: " << b1->getName() << endl;
                    cout << "Kontonummer: " << k1->getKontonummer() << endl;
                    cout << "Kontoart: " << k1->getKontoArt() << endl;
                    cout << "Kontostand: " << k1->getKontostand() << " Euro" << endl;
                    break;
                }

                case 2: {
                    vector<Konto*>& konten = b1->getKonten();

                    if (konten.empty()) {
                        cout << "Kein Konto vorhanden." << endl;
                        break;
                    }
                    int nummer;
                    double betrag;
                    nummer = leseInt("Kontonummer: ");
                    Konto* konto = b1->MykontoSuchen(nummer);

                    if (konto == nullptr) {
                        cout << "Sie haben keinen Zugriff auf dieses Konto oder das Konto wurde nicht gefunden." << endl;
                        break;
                    }

                    betrag = leseDouble("Betrag zum Einzahlen: ");

                    if (konto->einzahlen(betrag)) {
                        b.transaktionSpeichern(
                            "Einzahlung",
                            betrag,
                            "Einzahlung auf Konto " + to_string(nummer)
                        );
                        cout << "Einzahlung erfolgreich." << endl;
                    }
                   
                    break;
                }

                case 3: {
                    vector<Konto*>& konten = b1->getKonten();

                    if (konten.empty()) {
                        cout << "Kein Konto vorhanden." << endl;
                        break;
                    }
                    int nummer;
                    double betrag;
                    nummer = leseInt("Kontonummer: ");
                    Konto* konto = b1->MykontoSuchen(nummer);

                    if (konto == nullptr) {
                        cout << "Sie haben keinen Zugriff auf dieses Konto oder das Konto wurde nicht gefunden." << endl;
                        break;
                    }

                    betrag = leseDouble("Betrag zum Auszahlen: ");

                    if (konto->auszahlen(betrag)) {
                        b.transaktionSpeichern(
                            "Auszahlung",
                            betrag,
                            "Auszahlung von Konto " + to_string(nummer)
                        );
                        cout << "Auszahlung erfolgreich." << endl;
                    }
                  
                    break;
                }

                case 4: {
                    vector<Konto*>& konten = b1->getKonten();

                    for (int i = 0; i < konten.size(); i++) {
                        cout << "\nKonto " << i + 1 << endl;
                        cout << "Inhaber: " << konten[i]->getInhaber() << endl;
                        cout << "Kontonummer: " << konten[i]->getKontonummer() << endl;
                        cout << "Kontostand: " << konten[i]->getKontostand() << " Euro" << endl;
                        cout << "Kontoart: " << konten[i]->getKontoArt() << endl;
                    }
                    break;
                }

                case 5: {
                    vector<Konto*>& konten = b1->getKonten();

                    for (int i = 0; i < konten.size(); i++) {
                        cout << "\n<<Historie fuer Konto -> " << konten[i]->getKontonummer() << endl;
                        konten[i]->zeigeHistorie();
                    }
                    break;
                }

                case 6: {
                    int von, nach;
                    double betrag;

                    von = leseInt("Von Kontonummer: ");
                    Konto* sender = b1->MykontoSuchen(von);
                   

                    if (sender == nullptr) {
                        cout << "Sie haben keinen Zugriff auf dieses Konto oder das Konto wurde nicht gefunden." << endl;
                        break;
                    }
                    if (!sender->ueberweisungErlaubt()) {
                        cout << "Ueberweisungen sind fuer dieses Sparkonto standardmaessig gesperrt und muessen durch die Bank freigegeben werden." << endl;;
                        break;
                    }
                    nach = leseInt("Nach Kontonummer: ");
                    Konto* empfaenger = b.KontoSuchen(nach);
                    if (empfaenger == nullptr) {
                        cout << "Empfaengerkonto nicht gefunden." << endl;
                        break;
                    }
                    betrag = leseDouble("Betrag: ");

                    b.ueberweisen(von, nach, betrag);
                    break;
                }

                case 7:
                    break;

                default:
                    cout << "Ungueltige Auswahl." << endl;
                    break;
                }

                if (auswahl == 7) {
                    break;
                }
            }

            break;
        }

        case 2: {
            int bankAuswahl;

            while (true) {
                cout << "\n--- Bank-Administration ---" << endl;
                cout << "1. Konto suchen" << endl;
                cout << "2. Neues Konto erstellen" << endl;
                cout << "3. Alle Konten anzeigen" << endl;
                cout << "4. Ueberweisen" << endl;
                cout << "5. Alle Transaktionen anzeigen" << endl;
                cout << "6. Einzahlung" << endl;
                cout << "7. Auszahlung" << endl;
                cout << "8. Konto sperren" << endl;
                cout << "9. Konto entsperren" << endl;
                cout << "10. Zurueck" << endl;
                bankAuswahl = leseInt("Auswahl: ");

                switch (bankAuswahl) {

                case 1: {
                    int nummer;
                    nummer = leseInt("Kontonummer eingeben: ");
                    Konto* gefunden = b.KontoSuchen(nummer);

                    if (gefunden == nullptr) {
                        cout << "Konto nicht gefunden." << endl;
                    }
                    else {
                        cout << "Konto gefunden." << endl;
                        cout << "Inhaber: " << gefunden->getInhaber() << endl;
                        cout << "Kontonummer: " << gefunden->getKontonummer() << endl;
                        cout << "Kontostand: " << gefunden->getKontostand() << " Euro" << endl;
                    }
                    break;
                }

                case 2: {
                    double betrag;
                    string inhaberName;

                    cout << "Name des Kontoinhabers: ";
                    cin >> inhaberName;

                    cout << "1. Standardkonto" << endl;
                    cout << "2. Girokonto" << endl;
                    cout << "3. Sparkonto" << endl;
                    int kontoTyp;

                    while (true) {
                        kontoTyp = leseInt("Kontoart waehlen: ");

                        if (kontoTyp >= 1 && kontoTyp <= 3) {
                            break;
                        }

                        cout << "Ungueltige Kontoart. Bitte 1, 2 oder 3 eingeben." << endl;
                    }

                    betrag = leseDouble("Startbetrag fuer neues Konto:");

                    Konto* k1 = nullptr;

                    if (kontoTyp == 1) {
                        k1 = new Konto(betrag, inhaberName, "Standardkonto");
                    }
                    else if (kontoTyp == 2) {
                        k1 = new Girokonto(betrag, inhaberName);
                    }
                    else {
                        k1 = new Sparkonto(betrag, inhaberName);
                    }

                    b.KontoHinzufuegen(k1);

                    cout << "Neues Bankkonto wurde erstellt." << endl;
                    cout << "Kontoart: " << k1->getKontoArt() << endl;
                    cout << "Kontoinhaber: " << inhaberName << endl;
                    cout << "Kontonummer: " << k1->getKontonummer() << endl;
                    cout << "Kontostand: " << betrag << " Euro" << endl;

                    break;
                }

                case 3: {
                    b.alleKontenAnzeigen();
                    break;
                }
                case 4: {
                    int von, nach;
                    double betrag;
            
                    von = leseInt("Von Kontonummer: ");
                    Konto* sender = b.KontoSuchen(von);
                    if (sender == nullptr) {
                        cout << "Konto nicht gefunden!" << endl;
                        break;
                    }
                    nach = leseInt("Nach Kontonummer: ");
                    Konto* empfaenger = b.KontoSuchen(nach);
                    if (empfaenger == nullptr) {
                        cout << "Empfaengerkonto nicht gefunden." << endl;
                        break;
                    }
                    betrag = leseDouble("Betrag: ");
                    b.ueberweisen(von, nach, betrag);
                    break;

                }
                case 5: {
                    b.zeigeBankHistorie();
                    break;
                }
                case 6: {
                    int nummer;
                    double betrag;

                    nummer = leseInt("Kontonummer: ");

                    Konto* konto = b.KontoSuchen(nummer);

                    if (konto == nullptr) {
                        cout << "Konto nicht gefunden." << endl;
                        break;
                    }

                    betrag = leseDouble("Betrag zum Einzahlen: ");

                    if (konto->einzahlen(betrag)) {
                        b.transaktionSpeichern(
                            "Einzahlung",
                            betrag,
                            "Einzahlung auf Konto " + to_string(nummer)
                        );
                        cout << "Einzahlung erfolgreich." << endl;
                    }

                    
                    break;
                }

                case 7: {
                    int nummer;
                    double betrag;

                    nummer = leseInt("Kontonummer: ");

                    Konto* konto = b.KontoSuchen(nummer);

                    if (konto == nullptr) {
                        cout << "Konto nicht gefunden." << endl;
                        break;
                    }

                    betrag = leseDouble("Betrag zum Auszahlen: ");

                    if (konto->auszahlen(betrag)) {
                        b.transaktionSpeichern(
                            konto->getKontoArt() + "-Auszahlung",
                            betrag,
                            "Auszahlung von " + konto->getKontoArt() + " Konto " + to_string(nummer)
                        );
                        cout << "Auszahlung erfolgreich." << endl;
                    }

                   
                    break;
                }
                case 8: {
                    int nummer;
                    nummer = leseInt("Kontonummer zum Sperren: ");

                    Konto* konto = b.KontoSuchen(nummer);

                    if (konto == nullptr) {
                        cout << "Konto nicht gefunden." << endl;
                        break;
                    }

                    konto->sperren();

                    cout << "Konto wurde gesperrt." << endl;

                    b.transaktionSpeichern(
                        "Konto-Sperrung",
                        0,
                        "Konto " + to_string(nummer) + " wurde gesperrt"
                    );

                    break;
                }
                case 9: {
                    int nummer;

                    nummer = leseInt("Kontonummer zum Entsperren: ");

                    Konto* konto = b.KontoSuchen(nummer);

                    if (konto == nullptr) {
                        cout << "Konto nicht gefunden." << endl;
                        break;
                    }

                    konto->entsperren();

                    cout << "Konto wurde entsperrt." << endl;
                    Sparkonto* sparkonto = dynamic_cast<Sparkonto*>(konto);

                    if (sparkonto != nullptr) {
                        sparkonto->ueberweisungFreigeben();
                    }

                    b.transaktionSpeichern(
                        "Konto-Entsperrung",
                        0,
                        "Konto " + to_string(nummer) + " wurde entsperrt"
                    );

                    break;
                }

                case 10:
                    break;

                default:
                    cout << "Ungueltige Auswahl." << endl;
                    break;
                }

                if (bankAuswahl == 10) {
                    break;
                }
            }

            break;
        }

        case 3:
            return 0;

        default:
            cout << "Ungueltige Auswahl." << endl;
            break;
        }
    }

    return 0;
}