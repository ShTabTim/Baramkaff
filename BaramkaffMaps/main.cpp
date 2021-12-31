#include <iostream>
#include <string>
#include "FundLibs/FileHelper/FileHelper.hpp"

int main() {
	setlocale(LC_ALL, "ru");

	size_t XX, YY, ZZ;

	std::cout << "¬ведите XX:\n";
	std::cin >> XX;
	std::cout << "¬ведите YY:\n";
	std::cin >> YY;
	std::cout << "¬ведите ZZ:\n";
	std::cin >> ZZ;

	std::string file_name = std::to_string(XX) + "-" + std::to_string(YY) + "-" + std::to_string(ZZ) + ".voa";

	std::cout << "»м€ файла : " << file_name;
	file_name = "/Map/ " + file_name;
	std::cout << "ѕуть файла : " << file_name;

	wrFile(file_name.c_str(), 0);

	//int a9371;
	//std::cin >> a9371;

	return 0;
}