#include "../../include/structs.h"

Array2D::Array2D(const int __rows, const int __cols) : rows(__rows), cols(__cols) {
	array = new int*[rows];

	for (std::size_t i = 0; i < rows; i++) {
		array[i] = new int[cols];
	}
}

// Copy constructor
Array2D::Array2D(const Array2D& other) : rows(other.rows), cols(other.cols) {
	array = new int*[rows];

	for (std::size_t i = 0; i < rows; i++) {
		array[i] = new int[cols];
	}

	for (std::size_t i = 0; i < rows; i++) {
		for (std::size_t j = 0; j < cols; j++) {

			array[i][j] = other.array[i][j];

		}
	}
}

Array2D &Array2D::operator=(const Array2D &other) {
	rows = other.rows;
	cols = other.cols;

	array = new int*[rows];

	for (std::size_t i = 0; i < rows; i++) {
		array[i] = new int[cols];
	}

	for (std::size_t i = 0; i < rows; i++) {
		for (std::size_t j = 0; j < cols; j++) {

			array[i][j] = other.array[i][j];

		}
	}

	return *this;
}

// Manual call for destructor
void Array2D::Delete() {
	for (std::size_t i = 0; i < rows; i++) {
		delete[] array[i];
	}

	delete[] array;
}

std::string Array2D::ToString() {
	std::string string = "[";

	for (std::size_t i = 0; i < rows; i++) {
		if (i != 0) {
			string += " ";
		}

		string += "[";

		for (std::size_t j = 0; j < cols; j++) {
			string += " " + std::to_string(array[i][j]);
		}

		string += " ]";

		if (i + 1 != rows) {
			string += "\n";
		}
	}
	string += "]";

	return string;
}


int* Array2D::operator[](const int index) {
	return array[index];
}

int* Array2D::operator[](const size_t index) {
	return array[index];
}