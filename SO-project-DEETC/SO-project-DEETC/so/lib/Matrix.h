#ifndef MATRIX
#define MATRIX

#include <vector>
#include <iostream>
#include <assert.h>

using namespace std;


class Matrix {

public:
    // Constructors
	Matrix();
	Matrix(int nlines, int ncols);
    // Public interface
    vector<int>& getVec();
    int getVal(int i, int j) const;
	int nLines() const;
	int nCols() const;
	void setDimensions(int nlines, int ncols);
    void reserve(int nlines, int ncols);
	friend ostream& operator<<(ostream& os, const Matrix& Matrix);

private:
    int nlines, ncols;
    vector<int> vec;
};


inline Matrix::Matrix() : nlines(0), ncols(0), vec(0) { }

//Matrix::Matrix(int nlines, int ncols) : nlines(nlines), ncols(ncols), vec(nlines*ncols) {
inline Matrix::Matrix(int nlines, int ncols) : nlines(nlines), ncols(ncols) {
//    cout << *this << endl;
}

inline vector<int>& Matrix::getVec() { return vec; }




inline int Matrix::getVal(int i, int j) const {
//    int i0 = i, j0 = j;  /// Exams indexes start at 0 as specified by the ITC 2007 rules
    int i0 = i-1, j0 = j-1;  /// Exams's indexes start at 1 - Toronto?
//    cout << "[i, j] = [" << i0 << ", " << j0 << "]" << endl;
//    assert(i0 >= 0 && i0 < nlines && j0 >= 0 && j0 < ncols);
    return vec[i0*ncols+j0];
}

inline int Matrix::nLines() const { return nlines; }

inline int Matrix::nCols() const { return ncols; }

inline void Matrix::setDimensions(int nlines, int ncols) {
    this->nlines = nlines;
    this->ncols = ncols;
    //vec.resize(nlines*ncols);
}

inline void Matrix::reserve(int nlines, int ncols) {
    this->nlines = nlines;
    this->ncols = ncols;
    vec.resize(nlines*ncols);
}



#endif
