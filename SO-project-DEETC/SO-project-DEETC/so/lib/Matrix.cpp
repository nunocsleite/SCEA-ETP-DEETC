

#include "Matrix.h"



ostream& operator<<(ostream& os, const Matrix& matrix) {
	os << endl << "Matrix" << endl;
	os << "nlines = " << matrix.nLines() << ", ncols = " << matrix.nCols() << endl;
	os << "size = " << matrix.nLines()*matrix.nCols() << endl;
	os << "vec.size = " << matrix.vec.size() << endl;

    for (int i = 0; i < matrix.nLines(); ++i) {
        for (int j = 0; j < matrix.nCols(); ++j) {
            os << matrix.getVal(i, j) << " ";
        }
        os << endl;
    }
	return os;
}

