#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include "matrix.h"
#include "basic-matrix.h"
#include "matrix-ex.h"
#include <cstring>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <set>

namespace mat {
    namespace ex {
        class MatrixException;
        class MismatchedSizeException;
        class DuplicatedTripleException;
        class NotSquareException;
        class NoInverseException;
        class InvalidSizeException;
    }
    template<typename T>
    struct Triple {
        long _row;
        long _col;
        T val;

        bool operator==(const Triple<T> & right){
            if (this->_row == right._row && this->_col == right._col) {
                return true;
            }else {
                return false;
            }
        }
    };

    template<typename>
    class Matrix;

    template<typename>
    class BasicMatrix;

    template<class T>
    class SparseMatrix : public Matrix<T> {
    private:
        long size;
        std::set<Triple<T>> triples;

        inline void addEle(Triple<T> tri) {
            triples.insert(tri);
        }
    public:
        SparseMatrix(int, int);

        // SparseMatrix(const cv::Mat &mat);

        SparseMatrix(std::vector<std::vector<T>>);

        SparseMatrix(int, int, T*);

        SparseMatrix(int, int, std::vector<Triple<T>>);

        SparseMatrix(int, int, std::set<Triple<T>>);

        SparseMatrix(const SparseMatrix<T> &);

        SparseMatrix<T>& operator=(const SparseMatrix<T> &);

        void add(const BasicMatrix<T> &);

        void add(const SparseMatrix<T> &);

        void subtract(const BasicMatrix<T> &);

        void subtract(const SparseMatrix<T> &);

        void scalarMultiply(T);

        void scalarDivide(T);

        void scalarDivide(std::complex<long double>);

        void dotProduct(const BasicMatrix<T> &);

        void dotProduct(const SparseMatrix<T> &);

        void crossProduct(const BasicMatrix<T> &);

        void crossProduct(const SparseMatrix<T> &);

        void transpose();

        void inverse();

        void reverse();

        void conjugate();

        T getMax();

        T getMin();

        T getSum();

        T getAvg();

        T getRowMax(int);

        T getColMax(int);

        T getRowMin(int);

        T getColMin(int);

        T getRowSum(int);

        T getColSum(int);

        T getRowAvg(int);

        T getColAvg(int);

        T getEigenvalue();

        Matrix<T> &getEigenvector();

        T getByIndex(int _row, int _col) const;

        void setByIndex(int _row, int _col, T val);

        T getTrace();

        T getDeterminant();

        void reshape(int row, int col);

        void sliceRow(int row1, int row2);

        void sliceCol(int col1, int col2);

        void slice(int row1, int row2, int col1, int col2);

        Matrix<T> &convolve(BasicMatrix<T> &, int stride = 1, int padding = 0);

        Matrix<T> &convolve(SparseMatrix<T> &, int stride = 1, int padding = 0);

        void exponent(int exp);

        std::set<Triple<T>> getTriples() const{
            return this->triples;
        }
    };

    template<class T>
    SparseMatrix<T>::SparseMatrix(int row, int col): Matrix<T>(row, col) {}

    // template<class T>
    // SparseMatrix<T>::SparseMatrix(const cv::Mat &mat) {

    // }

    template<class T>
    SparseMatrix<T>::SparseMatrix(std::vector<std::vector<T>> mat): Matrix<T>(mat.size(), mat[0].size()) {
        for (size_t i = 0; i < mat.size(); i++)
        {
            for (size_t j = 0; j < mat[i].size(); j++)
            {
                if (mat[i][j] != 0) {
                    Triple<T> triple(i, j, mat[i][j]);
                    triples.insert(triple);
                }
            }
            
        }
        
    }

    template<class T>
    SparseMatrix<T>::SparseMatrix(int row, int col, T* _data): Matrix<T>(row, col){
        for (size_t i = 0; i < this->getSize(); i++)
        {
            if (_data[i] != 0) {
                Triple<T> triple(i/this->getCol(), i%this->getCol(), _data[i]);
                triples.insert(triple);
            }
        }
    }

    template<class T>
    SparseMatrix<T>::SparseMatrix(int row, int col, std::vector<Triple<T>> mat): Matrix<T>(row, col){
        std::set<Triple<T>> temp(mat.begin(), mat.end());
        if (mat.size() != temp.size()) {
            throw ex::DuplicatedTripleException();
        }
        mat.assign(temp.begin(), temp.end());
        this->triples(mat);
    }

    template<class T>
    SparseMatrix<T>::SparseMatrix(int row, int col, std::set<Triple<T>> mat): Matrix<T>(row, col){
        this->triples(mat);
    }

    template<class T>
    SparseMatrix<T>::SparseMatrix(const SparseMatrix<T> & right): Matrix<T>(right.getRow(), right.getCol()){
        this->triples(right.triples);
    }

    template<class T>
    SparseMatrix<T>& SparseMatrix<T>::operator=(const SparseMatrix<T> & right){
        this->setSize(right.getSize());
        this->setRow(right.getRow());
        this->setCol(right.getCol());
        this->triples(right.triples);
    }

    template <class T>
    T SparseMatrix<T>::getByIndex(int _row, int _col) const {
        T target=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (this->triples[i]._col==_col &&this->triples[i]._row==_row)
            {
                return target=this->triples[i].val;
            }
            
        }
        return target;
    }

    template <class T>
    void SparseMatrix<T>::setByIndex(int _row, int _col, T val) {
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (this->triples[i]._row==_row&&this->triples[i]._col==_col)
            {
                this->triples[i].val=val;
                return;
            }
        }
        Triple<T>triple(_row,_col,val);
        this->addEle(triple);
        return;
    }

    template<class T>
    void SparseMatrix<T>::add(const BasicMatrix<T> &) {
    }

    template<class T>
    void SparseMatrix<T>::add(const SparseMatrix<T> &right) {
        SparseMatrix<T> mat(this->row, this->col);
        for (int i = 0; i < this->triples.size(); i++) {
            for (int j = 0; j < right.triples.size(); j++) {
                Triple<T> LP = triples[i];
                Triple<T> RP = triples[j];
                if (LP._row == RP._row && LP._row == RP._row) {
                    mat.setByIndex(LP._row, LP._col, LP.val + RP.val);
                }
            }
        }
        *this = mat;
    }

    template<class T>
    void SparseMatrix<T>::subtract(const BasicMatrix<T> &) {
    }

    template<class T>
    void SparseMatrix<T>::subtract(const SparseMatrix<T> &right) {
        SparseMatrix<T> mat(this->row, this->col);
        for (int i = 0; i < this->triples.size(); i++) {
            for (int j = 0; j < right.triples.size(); j++) {
                Triple<T> LP = triples[i];
                Triple<T> RP = triples[j];
                if (LP._row == RP._row && LP._row == RP._row) {
                    mat.setByIndex(LP._row, LP._col, LP.val - RP.val);
                }
            }
        }
        *this = mat;
    }

    template<class T>
    void SparseMatrix<T>::scalarMultiply(T x) {
        for (int i = 0; i < this->triples.size(); i++) {
            this->triples[i] *= x;
        }
    }

    template<class T>
    void SparseMatrix<T>::scalarDivide(T x) {
        for (int i = 0; i < this->triples.size(); i++) {
            this->triples[i] /= x;
        }
    }

    template<class T>
    void SparseMatrix<T>::dotProduct(const BasicMatrix<T> &) {

    }

    template<class T>
    void SparseMatrix<T>::dotProduct(const SparseMatrix<T> &right) {
        if (this->row != right.row) {
            throw ex::MismatchedSizeException(*this, right, "matrix dot product");
        } else if (this->col != 1 && this->col != right.col) {
            throw ex::MismatchedSizeException(*this, right, "matrix dot product");
        }

        int r = right.row;
        int c = right.col;
        if (this->col == 1) {
            SparseMatrix<T> mat(r, 1);
            for (int i = 0; i < this->triples.size(); i++) {
                for (int j = 0; j < right.triples.size(); j++) {
                    mat::Triple<T>& l_point = this->triples[i];
                    mat::Triple<T>& r_point = right.triples[j];
                    if (l_point._row == r_point._row) {
                        mat.setByIndex(r_point._row, r_point._col, l_point.val * r_point.val);
                    }
                }
            }
            *this = mat;
        } else {
            SparseMatrix<T> mat(r, c);
            for (int i = 0; i < this->triples.size(); i++) {
                for (int j = 0;j  < right.triples.size(); j++) {
                    mat::Triple<T>& l_point = this->triples[i];
                    mat::Triple<T>& r_point = right.triples[i];
                    if (l_point._row == r_point._row &&
                        l_point._col == r_point._col) {
                            mat.setByIndex(r_point._row, r_point._col, l_point.val * r_point.val);
                        }
                }
            }
            *this = mat;
        }
    }

    template<class T>
    void SparseMatrix<T>::crossProduct(const BasicMatrix<T> &) {

    }

    template<class T>
    void SparseMatrix<T>::crossProduct(const SparseMatrix<T> &right) {
        if (this->col != right.col) {
            throw ex::MismatchedSizeException(*this, right, "matrix cross product");
        }
        int r = this->row;
        int c = right.col;
        SparseMatrix<T> mat(r, c);
        for (int i = 0; i < this->triples.size(); i++) {
            Triple<T>& l_point = this->triples[i];
            for (int j = 0; j < right.triples.size(); j++) {
                Triple<T>& r_point = right.triples[j];
                if (l_point._col == r_point._row) {
                    mat.setByIndex(l_point._row, r_point._col, mat.getByIndex(l_point._row, r_point._col) + l_point.val * r_point.val);
                }
            }
        }
        *this = mat;
    }

    template<class T>
    void SparseMatrix<T>::transpose() {

    }

    template<class T>
    void SparseMatrix<T>::inverse() {

    }

    template<class T>
    void SparseMatrix<T>::reverse() {
        
    }

    template<class T>
    void SparseMatrix<T>::conjugate() {

    }

    template<class T>
    T SparseMatrix<T>::getMax() {
        T max=0;
        for (int i = 0;i< this->triples.size(); i++)
        {
            if (max<this->triples[i].val)
            {
                max=this->triples[i].val;
            }
            
        }
        return max;
    }

    template<class T>
    T SparseMatrix<T>::getMin() {
        T min=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (min>this->triples[i].val)
            {
                min=this->triples[i].val;
            }
            
        }
        return min;
        
        
    }

    template<class T>
    T SparseMatrix<T>::getSum() {
        T sum=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            sum+=this->triples[i].val;
        }
        return sum;
        
    }

    template<class T>
    T SparseMatrix<T>::getAvg() {
        return this->getSum/(this->col*this->row);
    }

    template<class T>
    T SparseMatrix<T>::getRowMax(int row) {
        T max=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (max<this->triples[i].val&&this->triples[i]._row==row)
            {
                max=this->triples[i].val;
            }
            
        }
        return max;
    }

    template<class T>
    T SparseMatrix<T>::getColMax(int col) {
        T max=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (max<this->triples[i].val&&this->triples[i]._col==col)
            {
                max=this->triples[i].val;
            }
            
        }
        return max;
    }

    template<class T>
    T SparseMatrix<T>::getRowMin(int row) {
        T min=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (min>this->triples[i].val&&this->triples[i]._row==row)
            {
                min=this->triples[i].val;
            }
            
        }
        return min;

    }

    template<class T>
    T SparseMatrix<T>::getColMin(int col) {
         T min=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (min>this->triples[i].val&&this->triples[i]._col==col)
            {
                min=this->triples[i].val;
            }
            
        }
        return min;
    }

    template<class T>
    T SparseMatrix<T>::getRowSum(int row) {
        T sum=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (this->triples[i]._row==row)
            {
                sum+=this->triples[i].val;
            }
            
        }
        return sum;
    }

    template<class T>
    T SparseMatrix<T>::getColSum(int col) {
        T sum=0;
        for (int i = 0; i < this->triples.size(); i++)
        {
            if (this->triples[i]._col==col)
            {
                sum+=this->triples[i].val;
            }
            
        }
        return sum;
    }

    template<class T>
    T SparseMatrix<T>::getRowAvg(int row) {
        return this->getRowSum(row)/this->col;
    }

    template<class T>
    T SparseMatrix<T>::getColAvg(int col) {
        return this->getColSum(col)/this->row;
    }

    template<class T>
    T SparseMatrix<T>::getEigenvalue() {
    }

    template<class T>
    Matrix<T> &SparseMatrix<T>::getEigenvector() {
    }

    template<class T>
    T SparseMatrix<T>::getTrace() {
    }

    template<class T>
    T SparseMatrix<T>::getDeterminant() {
    }

    template<class T>
    void SparseMatrix<T>::reshape(int row, int col) {
        long _size = row * col;
    if (this->getSize() == _size) {
        this->setRow(col);
        this->setCol(row);
        
        
    } else
        throw ex::MismatchedSizeException(this->getRow(), this->getCol(), row, col, "matrix reshaping");
        
    }

    template<class T>
    void SparseMatrix<T>::sliceRow(int row1, int row2) {

    }

    template<class T>
    void SparseMatrix<T>::sliceCol(int col1, int col2) {

    }

    template<class T>
    void SparseMatrix<T>::slice(int row1, int row2, int col1, int col2) {

    }

    template<class T>
    Matrix<T> &SparseMatrix<T>::convolve(BasicMatrix<T> &, int stride, int padding) {
    }

    template<class T>
    Matrix<T> &SparseMatrix<T>::convolve(SparseMatrix<T> &, int stride, int padding) {
    }

    template<class T>
    void SparseMatrix<T>::exponent(int exp){
        
    }

}

#endif