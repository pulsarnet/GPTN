//
// Created by nmuravev on 1/21/2022.
//

#include <memory>
#include "rust.h"

using namespace ffi;

extern "C" {

    // PetriNetContext
    PetriNetContext* new_context();
    PetriNet* ctx_net(const PetriNetContext*);
    DecomposeContext* ctx_decompose_context(const PetriNetContext*);
    void ctx_set_decompose_context(PetriNetContext*, DecomposeContext*);
    void ctx_decompose(PetriNetContext&);
    void delete_context(PetriNetContext*);

    // PetriNet
    PetriNet* create_net();
    void delete_net(PetriNet*);
    void net_positions(const PetriNet& self, CVec<Vertex*>* return$);
    void net_transitions(const PetriNet& self, CVec<Vertex*>* return$);
    void net_connections(const PetriNet& self, CVec<Connection*>* return$);
    Vertex* add_position(PetriNet&);
    Vertex* add_position_with(PetriNet&, usize);
    Vertex* add_position_with_parent(PetriNet&, usize, usize);
    Vertex* get_position(PetriNet&, usize);
    void remove_position(PetriNet&, Vertex*);
    Vertex* add_transition(PetriNet&);
    Vertex* add_transition_with(PetriNet&, usize);
    Vertex* add_transition_with_parent(PetriNet&, usize, usize);
    Vertex* get_transition(PetriNet&, usize);
    void remove_transition(PetriNet&, Vertex*);
    void connect_vertexes(PetriNet&, Vertex*, Vertex*);
    void remove_connection(PetriNet&, Vertex*, Vertex*);
    void petri_net_as_matrix(const PetriNet&, CNamedMatrix**, CNamedMatrix**);

    Vertex* net_get_vertex(const PetriNet&, VertexIndex);

    // Vertex
    VertexIndex vertex_index(const Vertex&);
    usize vertex_markers(const Vertex&);
    void vertex_add_marker(Vertex&);
    void vertex_remove_marker(Vertex&);
    void vertex_set_markers(Vertex&, usize);
    char* vertex_label(const Vertex&, bool);
    void vertex_set_label(Vertex&, char*);
    VertexType vertex_type(const Vertex&);
    usize vertex_parent(const Vertex&);
    void vertex_set_parent(Vertex&, VertexIndex);

    // Connection
    VertexIndex connection_from(const Connection& self);
    VertexIndex connection_to(const Connection& self);

    // DecomposeContext
    DecomposeContext* decompose_context_init(PetriNet&);
    DecomposeContext* decompose_context_from_nets(PetriNet* const *, usize);
    void decompose_context_delete(DecomposeContext*);
    usize decompose_context_positions(DecomposeContext&);
    usize decompose_context_transitions(DecomposeContext&);
    CMatrix* decompose_context_primitive_matrix(DecomposeContext&);
    PetriNet* decompose_context_primitive_net(DecomposeContext&);
    usize decompose_context_position_index(DecomposeContext&, usize);
    usize decompose_context_transition_index(DecomposeContext&, usize);
    PetriNet* decompose_context_linear_base_fragments(DecomposeContext&);
    void decompose_context_parts(const DecomposeContext&, CVec<PetriNet*>* return$);
    usize synthesis_programs(const DecomposeContext&);
    usize synthesis_program_size(const DecomposeContext&, usize);
    void synthesis_add_program(DecomposeContext&);
    void synthesis_remove_program(DecomposeContext&, usize);
    usize synthesis_program_value(const DecomposeContext&, usize, usize);
    void synthesis_set_program_value(DecomposeContext&, usize, usize, usize);
    char* synthesis_program_header_name(const DecomposeContext&, usize, bool);
    CMatrix* synthesis_c_matrix(const DecomposeContext&);
    PetriNet* synthesis_eval_program(DecomposeContext&, usize);
    PetriNet* synthesis_program_net_after(const DecomposeContext&, usize);
    PetriNet* synthesis_init_program_net_after(DecomposeContext&, usize);

    // CMatrix
    i32 matrix_index(const CMatrix&, usize, usize);
    void matrix_set_value(CMatrix&, usize, usize, i32);
    usize matrix_rows(const CMatrix&);
    usize matrix_columns(const CMatrix&);

    // CNamedMatrix
    i32 named_matrix_index(const CNamedMatrix&, usize, usize);
    char* named_matrix_column_name(const CNamedMatrix&, usize);
    char* named_matrix_row_name(const CNamedMatrix&, usize);
    usize named_matrix_rows(const CNamedMatrix&);
    usize named_matrix_columns(const CNamedMatrix&);

    // CVec<u64>
    usize vec_len_u64(const CVec<usize>* self);
    const usize* vec_data_u64(const CVec<usize>* self);

    // CVec<Vertex>
    usize vec_len_vertex(const CVec<Vertex*>* self);
    Vertex* const* vec_data_vertex(const CVec<Vertex*>* self);

    // CVec<Connection>
    usize vec_len_connection(const CVec<Connection*>* self);
    Connection* const* vec_data_connection(const CVec<Connection*>* self);

    // CVec<PetriNet>
    usize vec_len_nets(const CVec<PetriNet*>* self);
    PetriNet* const* vec_data_nets(const CVec<PetriNet*>* self);

    //Helpers
    void remove_string(char*);
};

template<>
usize CVec<usize>::size() const noexcept {
    return ::vec_len_u64(this);
}

template<>
const usize* CVec<usize>::data() const noexcept {
    return ::vec_data_u64(this);
}

template<>
usize CVec<Vertex*>::size() const noexcept {
    return ::vec_len_vertex(this);
}

template<>
Vertex* const* CVec<Vertex*>::data() const noexcept {
    return ::vec_data_vertex(this);
}

template<>
const std::size_t CVec<Vertex *>::size_of() const noexcept {
    return sizeof(Vertex*);
}

template<>
usize CVec<Connection*>::size() const noexcept {
    return ::vec_len_connection(this);
}

template<>
Connection* const* CVec<Connection*>::data() const noexcept {
    return ::vec_data_connection(this);
}

template<>
const std::size_t CVec<Connection *>::size_of() const noexcept {
    return sizeof(Connection*);
}

template<>
usize CVec<PetriNet*>::size() const noexcept {
    return ::vec_len_nets(this);
}

template<>
PetriNet* const* CVec<PetriNet*>::data() const noexcept {
    return ::vec_data_nets(this);
}

template<>
const std::size_t CVec<PetriNet *>::size_of() const noexcept {
    return sizeof(PetriNet*);
}

PetriNetContext *PetriNetContext::create() {
    return ::new_context();
}

void PetriNetContext::free(PetriNetContext *context) {
    ::delete_context(context);
}

DecomposeContext *PetriNetContext::decompose_ctx() const {
    return ::ctx_decompose_context(this);
}

PetriNet *PetriNetContext::net() const {
    return ::ctx_net(this);
}

void PetriNetContext::decompose() {
    ::ctx_decompose(*this);
}

void PetriNetContext::set_decompose_ctx(DecomposeContext *ctx) {
    ::ctx_set_decompose_context(this, ctx);
}

PetriNet *PetriNet::create() {
    return ::create_net();
}

CVec<Vertex*> PetriNet::positions() const {
    CVec<Vertex*> result${};
    ::net_positions(*this, &result$);
    return result$;
}

CVec<Vertex*> PetriNet::transitions() const {
    CVec<Vertex*> result${};
    ::net_transitions(*this, &result$);
    return result$;
}

CVec<Connection *> PetriNet::connections() const {
    CVec<Connection*> result${};
    ::net_connections(*this, &result$);
    return result$;
}

Vertex *PetriNet::add_position() {
    return ::add_position(*this);
}

Vertex *PetriNet::add_position_with(usize index) {
    return ::add_position_with(*this, index);
}

Vertex *PetriNet::add_position_with_parent(usize index, usize parent) {
    return ::add_position_with_parent(*this, index, parent);
}

Vertex *PetriNet::get_position(usize index) {
    return ::get_position(*this, index);
}

void PetriNet::remove_position(Vertex *position) {
    return ::remove_position(*this, position);
}

Vertex *PetriNet::add_transition() {
    return ::add_transition(*this);
}

Vertex *PetriNet::add_transition_with(usize index) {
    return ::add_transition_with(*this, index);
}

Vertex *PetriNet::add_transition_with_parent(usize index, usize parent) {
    return ::add_transition_with_parent(*this, index, parent);
}

Vertex *PetriNet::get_transition(usize index) {
    return ::get_transition(*this, index);
}

void PetriNet::remove_transition(Vertex *transition) {
    return ::remove_transition(*this, transition);
}

void PetriNet::connect(Vertex *from, Vertex *to) {
    return ::connect_vertexes(*this, from, to);
}

void PetriNet::remove_connection(Vertex *from, Vertex *to) {
    return ::remove_connection(*this, from, to);
}

Vertex *PetriNet::getVertex(VertexIndex index) const {
    return ::net_get_vertex(*this, index);
}

std::pair<CNamedMatrix*, CNamedMatrix*> PetriNet::as_matrix() const {
    CNamedMatrix* first;
    CNamedMatrix* second;
    ::petri_net_as_matrix(*this, &first, &second);
    return {first, second};
}

void PetriNet::drop() {
    ::delete_net(this);
}

VertexIndex Vertex::index() const {
    return ::vertex_index(*this);
}

usize Vertex::markers() const {
    return ::vertex_markers(*this);
}

void Vertex::add_marker() {
    ::vertex_add_marker(*this);
}

void Vertex::remove_marker() {
    ::vertex_remove_marker(*this);
}

void Vertex::set_markers(ffi::usize markers) {
    ::vertex_set_markers(*this, markers);
}

char *Vertex::get_name(bool show_parent) const {
    return ::vertex_label(*this, show_parent);
}

void Vertex::set_name(char *name) {
    ::vertex_set_label(*this, name);
}

VertexType Vertex::type() const {
    return ::vertex_type(*this);
}

void Vertex::set_parent(VertexIndex index) {
    ::vertex_set_parent(*this, index);
}

usize Vertex::parent() const {
    return ::vertex_parent(*this);
}

VertexIndex Connection::from() const {
    return ::connection_from(*this);
}

VertexIndex Connection::to() const {
    return ::connection_to(*this);
}

DecomposeContext *DecomposeContext::init(PetriNet *net) {
    return ::decompose_context_init(*net);
}

DecomposeContext *DecomposeContext::fromNets(const QVector<PetriNet *>& data) {
    return ::decompose_context_from_nets(data.data(), data.size());
}

usize DecomposeContext::positions() {
    return ::decompose_context_positions(*this);
}

usize DecomposeContext::transitions() {
    return ::decompose_context_transitions(*this);
}

CMatrix *DecomposeContext::primitive_matrix() {
    return ::decompose_context_primitive_matrix(*this);
}

PetriNet *DecomposeContext::primitive_net() {
    return ::decompose_context_primitive_net(*this);
}

usize DecomposeContext::position_index(usize i) {
    return ::decompose_context_position_index(*this, i);
}

usize DecomposeContext::transition_index(usize i) {
    return ::decompose_context_transition_index(*this, i);
}

PetriNet *DecomposeContext::linear_base_fragments() {
    return ::decompose_context_linear_base_fragments(*this);
}

CVec<PetriNet *> DecomposeContext::parts() const {
    CVec<PetriNet*> result${};
    ::decompose_context_parts(*this, &result$);
    return result$;
}


usize DecomposeContext::programs() const {
    return ::synthesis_programs(*this);
}

usize DecomposeContext::program_size(usize index) const {
    return ::synthesis_program_size(*this, index);
}

void DecomposeContext::add_program() {
    ::synthesis_add_program(*this);
}

void DecomposeContext::remove_program(usize index) {
    ::synthesis_remove_program(*this, index);
}

usize DecomposeContext::program_value(usize program, usize index) const {
    return ::synthesis_program_value(*this, program, index);
}

void DecomposeContext::set_program_value(usize program, usize index, usize value) {
    ::synthesis_set_program_value(*this, program, index, value);
}

char *DecomposeContext::program_header_name(usize index, bool label) const {
    return ::synthesis_program_header_name(*this, index, label);
}

CMatrix *DecomposeContext::c_matrix() const {
    return ::synthesis_c_matrix(*this);
}

PetriNet *DecomposeContext::eval_program(usize index) {
    return ::synthesis_eval_program(*this, index);
}

PetriNet* DecomposeContext::program_net_after(usize index) const {
    return ::synthesis_program_net_after(*this, index);
}

PetriNet *DecomposeContext::init_program_after(usize index) {
    return ::synthesis_init_program_net_after(*this, index);
}

//QVariant SynthesisContext::toVariant() const {
//    QVariantHash result;
//    result["c_matrix"] = c_matrix()->toVariant();
//
//    QVariantList programsList;
//    for (int i = 0; i < programs(); i++) {
//        QVariantHash programHash;
//        programHash["net_after"] = program_net_after(i)->toVariant();
//
//        QVariantList data;
//        for (int j = 0; j < program_size(i); j++) {
//            data.push_back(program_value(i, j));
//        }
//        programHash["data"] = data;
//
//        programsList << programHash;
//    }
//
//    result["programs"] = programsList;
//
//    return result;
//}
//
//void SynthesisContext::fromVariant(const QVariant &data) {
//    auto map = data.toHash();
//
//    // Восстановим тензор преобразования
//    c_matrix()->fromVariant(map["c_matrix"]);
//
//    // Восстановим синтезированные программы
//    auto programsList = map["programs"].toList();
//    for (auto& program : programsList) {
//        add_program();
//
//        auto programHash = program.toHash();
//        auto net = init_program_after(programs() - 1);
//        net->fromVariant(programHash["net_after"]);
//
//        auto programValues = programHash["data"].toList();
//        for (int i = 0; i < programValues.size(); i++) {
//            set_program_value(programs() - 1, i, programValues[i].toInt());
//        }
//    }
//}

i32 CMatrix::index(usize row, usize col) const {
    return ::matrix_index(*this, row, col);
}

void CMatrix::set_value(usize row, usize col, i32 value) {
    ::matrix_set_value(*this, row, col, value);
}

usize CMatrix::rows() const {
    return ::matrix_rows(*this);
}

usize CMatrix::columns() const {
    return ::matrix_columns(*this);
}

i32 CNamedMatrix::index(usize row, usize column) const {
    return ::named_matrix_index(*this, row, column);
}

QString CNamedMatrix::horizontalHeader(usize column) const {
    auto name = ::named_matrix_column_name(*this, column);
    auto result = QString::fromStdString(std::string(name));
    ::remove_string(name);
    return result;
}

QString CNamedMatrix::verticalHeader(usize column) const {
    auto name = ::named_matrix_row_name(*this, column);
    auto result = QString::fromStdString(std::string(name));
    ::remove_string(name);
    return result;
}

usize CNamedMatrix::rows() const {
    return ::named_matrix_rows(*this);
}

usize CNamedMatrix::columns() const {
    return ::named_matrix_columns(*this);
}