//
// Book : Version du chapitre 10 de l'ouvrage.
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 06/02/ 2018 
// Commit en cours : Drill du chapitre - 10.11 - Reading a structured file
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"


// Librairie restreinte fournie par BS
#include "std_lib_facilities.h"

const int not_a_reading = -7777;
const int not_a_month = -1;
constexpr int implausible_min = -200;
constexpr int implausible_max = 200;

struct Day {  
	vector<double> hour{ vector<double>(24, not_a_reading) };
};

struct Month {  
	int month{ not_a_month };
	vector <Day> day{32};
};

struct Year {  
	int year;
	vector <Month> month{12};
};

struct Reading {  
	int day;
	int hour;
	double temperature;
};

int month_to_int(string s)
{
	vector<string>month_input_tbl = { "jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec" };
	for (int i = 0; i < 12; ++i)if (month_input_tbl[i] == s)return i;
	return -1;
}

string int_to_month(int i)
{
	vector<string>month_print_tbl = { "January","February","March","April","May","June","July","August","September","October","November","December" };
	if (i < 0 || 12 <= i)error("bad month index");
	return month_print_tbl[i];
}

void print_year(ofstream& os, Year& y)
{  
	// Cette fonction "imprime" dans le fichier de sortie les températures lues dans le fichier en entrée = Exo 5 page 377
	// Cette boucle peut sûrement être optimisée et moins "nested", sachant que les structures "en poupées russes" obligent à balayer 24 heures x 31 jours x 12 mois pour chaque année
		
	for (int i=0; i < y.month.size(); ++i) {
		for (int j=0; j < y.month[i].day.size(); ++j) {
			for (int k=0; k < y.month[i].day[j].hour.size(); ++k) {
				double temp = y.month[i].day[j].hour[k];
				if (temp!= not_a_reading) os <<"("<<y.year<<'-'<< int_to_month(y.month[i].month)<<") - température constatée à "<< k <<"h : " <<y.month[i].day[j].hour[k]<< '\n';
				
			}	
		}
	
	}

}

bool is_valid(const Reading& r)
{  
	if (r.day < 1 || 31 < r.day)return false;
	if (r.hour < 0 || 23 < r.hour)return false;
	if (r.temperature < implausible_min || implausible_max < r.temperature)return false;
	return true;
}


void end_of_loop(istream& ist,char term, const string& message) 
{  
	ist.exceptions(ist.exceptions() | ios_base::badbit);

	if (ist.fail()) {
		ist.clear();
		char ch;
		if(ist >> ch && ch == term) return;
		error(message);
	}
}


istream& operator>>(istream& is, Reading& r)
{  
	char ch1;
	if (is>>ch1 && ch1!='(') {
		is.unget();
		is.clear(ios_base::failbit);
		return is;
	}
	
	char ch2;
	int d;
	int h;
	double t;
	is >> d >> h >> t >> ch2;
	if (!is || ch2 != ')')error("bad reading");
	r.day = d;
	r.hour = h;
	r.temperature = t;
	return is;
}

istream& operator >> (istream& is, Month& m)
{  
	char ch = 0;
	if (is>>ch && ch!='{') {
		is.unget();
		is.clear(ios_base::failbit);
		return is;
	}
	
	string month_marker;
	string mm;
	is >> month_marker >> mm;
	if (!is || month_marker != "month")error("bad start of month");
	m.month = month_to_int(mm);

	int duplicates = 0;
	int invalids = 0;
	for (Reading r;is>>r;) {
		if (is_valid(r)) {
			if (m.day[r.day].hour[r.hour] != not_a_reading)++duplicates;
			m.day[r.day].hour[r.hour] = r.temperature;
			
		}
		else
			++invalids;
	
	}
	if (invalids)error("invalid readings in month", invalids);
	if (duplicates) error("duplicate readings in month", duplicates);
	end_of_loop(is, '}', "bad end of month");
	return is;
}

istream& operator>>(istream& is, Year& y)
{  
	char ch;
	is >> ch;
	if(ch!='{') {
		is.unget();
		is.clear(ios::failbit);
		return is;
	}

	string year_marker;
	int yy;
	is >> year_marker >> yy;
	if (!is || year_marker != "year")error("bad start of year");
	y.year = yy;

	while(true) {
		Month m;
		if (!(is >> m))break;
		y.month[m.month] = m;
	}
	end_of_loop(is, '}', "bad end of year");
	return is;
}



int main()
{
	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));

		cout << "Enter input file name\n";
		string iname;
		cin >> iname;
		ifstream ifs(iname);
		if (!ifs) error("can't open input file", iname);

		ifs.exceptions(ifs.exceptions() | ios_base::badbit);
		
		cout << "Enter output file name\n";
		string oname;
		cin >> oname;
		ofstream ofs(oname);
		if (!ofs) error("can't open output file", oname);

		vector<Year>ys;
		while(true) {
			Year y;
			if (!(ifs >> y))break;
			ys.push_back(y);
		}

		cout << "read " << ys.size() << " years of readings\n";

		for (Year& y : ys)print_year(ofs, y);


		keep_window_open();

		return 0;
	}
	
	/*
	catch (XXXXXXX::Invalid&)
	{
		cerr << "error: ???????????????????? ! \n";
		keep_window_open("q");	// For some Windows(tm) setup
		return 1;
	}
	*/

	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}
