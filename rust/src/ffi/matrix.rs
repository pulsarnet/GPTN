use std::ffi::CString;
use std::os::raw::c_char;
use indexmap::IndexMap;
use nalgebra::DMatrix;
use net::vertex::VertexIndex;
use Vertex;

#[repr(C)]
#[derive(Clone)]
pub struct CNamedMatrix {
    pub rows: Vec<String>,
    pub cols: Vec<String>,
    pub matrix: DMatrix<i32>,
}

impl CNamedMatrix {
    pub fn new(
        rows: &IndexMap<VertexIndex, Vertex>,
        cols: &IndexMap<VertexIndex, Vertex>,
        matrix: DMatrix<i32>,
    ) -> Self {
        CNamedMatrix {
            rows: rows.values().map(|v| v.name()).collect(),
            cols:  cols.values().map(|v| v.name()).collect(),
            matrix
        }
    }

    pub fn square(headers: Vec<String>, matrix: DMatrix<i32>) -> Self {
        CNamedMatrix {
            rows: headers.clone(),
            cols: headers,
            matrix
        }
    }
}

#[no_mangle]
extern "C" fn named_matrix_index(this: &CNamedMatrix, row: usize, column: usize) -> i32 {
    this.matrix.row(row)[column]
}

#[no_mangle]
extern "C" fn named_matrix_column_name(this: &CNamedMatrix, column: usize) -> *const c_char {
    let c_str = CString::new(this.cols[column].as_str()).unwrap();
    let c_str_ptr = c_str.as_ptr();
    std::mem::forget(c_str);
    c_str_ptr
}

#[no_mangle]
extern "C" fn named_matrix_row_name(this: &CNamedMatrix, row: usize) -> *const c_char {
    let c_str = CString::new(this.rows[row].as_str()).unwrap();
    let c_str_ptr = c_str.as_ptr();
    std::mem::forget(c_str);
    c_str_ptr
}

#[no_mangle]
extern "C" fn named_matrix_rows(this: &CNamedMatrix) -> usize {
    this.matrix.nrows()
}

#[no_mangle]
extern "C" fn named_matrix_columns(this: &CNamedMatrix) -> usize {
    this.matrix.ncols()
}

#[no_mangle]
extern "C" fn remove_string(this: *mut c_char) {
    unsafe { CString::from_raw(this) };
}