//
// Created by nmuravev on 1/21/2022.
//

#include <memory>
#include "rust.h"

using namespace ffi;

extern "C" {
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

    // Vertex
    usize vertex_index(const Vertex&);
    usize vertex_markers(const Vertex&);
    void vertex_add_marker(Vertex&);
    void vertex_remove_marker(Vertex&);
    char* vertex_get_name(const Vertex&);
    void vertex_set_name(Vertex&, char*);
    VertexType vertex_type(const Vertex&);
    usize vertex_parent(const Vertex&);

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

    // SynthesisContext
    SynthesisContext* synthesis_create(DecomposeContext&);
    SynthesisContext* synthesis_init(DecomposeContext&);
    void synthesis_delete(SynthesisContext*);
    usize synthesis_programs(const SynthesisContext&);
    usize synthesis_program_size(const SynthesisContext&, usize);
    void synthesis_add_program(SynthesisContext&);
    void synthesis_remove_program(SynthesisContext&, usize);
    usize synthesis_program_value(const SynthesisContext&, usize, usize);
    void synthesis_set_program_value(SynthesisContext&, usize, usize, usize);
    char* synthesis_program_header_name(const SynthesisContext&, usize, bool);
    CMatrix* synthesis_c_matrix(const SynthesisContext&);
    PetriNet* synthesis_eval_program(SynthesisContext&, usize);
    PetriNet* synthesis_program_net_after(const SynthesisContext&, usize);
    PetriNet* synthesis_init_program_net_after(SynthesisContext&, usize);
    DecomposeContext* synthesis_decompose_ctx(const SynthesisContext&);

    // CMatrix
    i32 matrix_index(const CMatrix&, usize, usize);
    void matrix_set_value(CMatrix&, usize, usize, i32);
    usize matrix_rows(const CMatrix&);
    usize matrix_columns(const CMatrix&);

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

QVariant PetriNet::toVariant() const {
    QVariantHash net;
    QVariantList vertexes;
    QVariantList v_connections;

    auto pos = positions();
    auto trans = transitions();
    auto conns = connections();

    for (int i = 0; i < pos.size(); i++) {
        auto position = pos[i];
        vertexes.push_back(position->toVariant());
    }

    for (int i = 0; i < trans.size(); i++) {
        auto transition = trans[i];
        vertexes.push_back(transition->toVariant());
    }

    for (int i = 0; i < conns.size(); i++) {
        auto connection = conns[i];

        v_connections.push_back(connection->toVariant());
    }

    net["vertex"] = vertexes;
    net["connections"] = v_connections;

    return net;
}

void PetriNet::fromVariant(const QVariant &data) {
    auto net = data.toHash();
    auto vertexes = net["vertex"].toList();
    auto connections = net["connections"].toList();

    for (const auto& vertexVariant : vertexes) {
        auto vertex = vertexVariant.toHash();
        auto index = vertex["index"].toInt();
        auto type = vertex["type"].toInt();
        auto label = vertex["label"].toString();
        auto parent_index = vertex["parent"].toInt();

        Vertex* added = nullptr;

        if (type == VertexType::Position) {
            if (!parent_index) {
                added = add_position_with(index);
            }
            else {
                added = add_position_with_parent(index, parent_index);
            }
            // TODO: set markers
        }
        else {
            if (!parent_index) {
                added = add_transition_with(index);
            }
            else {
                added = add_transition_with_parent(index, parent_index);
            }
        }

        added->set_name(label.toUtf8().data());
    }

    for (const auto& connection : connections) {
        auto conn = connection.toHash();
        auto from = conn["from"].toHash();
        auto to = conn["to"].toHash();

        auto from_type = from["type"].toInt();
        if (from_type == VertexType::Position) {
            connect(get_position(from["index"].toInt()), get_transition(to["index"].toInt()));
        }
        else {
            connect(get_transition(from["index"].toInt()), get_position(to["index"].toInt()));
        }
    }
}

void PetriNet::drop() {
    ::delete_net(this);
}

usize Vertex::index() const {
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

char *Vertex::get_name() const {
    return ::vertex_get_name(*this);
}

void Vertex::set_name(char *name) {
    ::vertex_set_name(*this, name);
}

VertexType Vertex::type() const {
    return ::vertex_type(*this);
}

usize Vertex::parent() const {
    return ::vertex_parent(*this);
}

QVariant Vertex::toVariant() const {
    QVariantHash vertex;
    vertex["type"] = type();
    vertex["index"] = index();
    vertex["label"] = get_name();
    vertex["parent"] = parent();

    if (type() == VertexType::Position) {
        vertex["markers"] = markers();
    }

    return vertex;
}

VertexIndex Connection::from() const {
    return ::connection_from(*this);
}

VertexIndex Connection::to() const {
    return ::connection_to(*this);
}

QVariant Connection::toVariant() const {
    auto f = from();
    auto t = to();

    QVariantHash from;
    from["type"] = f.type;
    from["index"] = f.id;

    QVariantHash to;
    to["type"] = t.type;
    to["index"] = t.id;

    QVariantHash connection;
    connection["from"] = from;
    connection["to"] = to;

    return connection;
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

void DecomposeContext::drop() {
    ::decompose_context_delete(this);
}

SynthesisContext *SynthesisContext::create(DecomposeContext *ctx) {
    return ::synthesis_create(*ctx);
}

SynthesisContext *SynthesisContext::init(DecomposeContext *ctx) {
    return ::synthesis_init(*ctx);
}

usize SynthesisContext::programs() const {
    return ::synthesis_programs(*this);
}

usize SynthesisContext::program_size(usize index) const {
    return ::synthesis_program_size(*this, index);
}

void SynthesisContext::add_program() {
    ::synthesis_add_program(*this);
}

void SynthesisContext::remove_program(usize index) {
    ::synthesis_remove_program(*this, index);
}

usize SynthesisContext::program_value(usize program, usize index) const {
    return ::synthesis_program_value(*this, program, index);
}

void SynthesisContext::set_program_value(usize program, usize index, usize value) {
    ::synthesis_set_program_value(*this, program, index, value);
}

char *SynthesisContext::program_header_name(usize index, bool label) const {
    return ::synthesis_program_header_name(*this, index, label);
}

CMatrix *SynthesisContext::c_matrix() const {
    return ::synthesis_c_matrix(*this);
}

PetriNet *SynthesisContext::eval_program(usize index) {
    return ::synthesis_eval_program(*this, index);
}

PetriNet* SynthesisContext::program_net_after(usize index) const {
    return ::synthesis_program_net_after(*this, index);
}

PetriNet *SynthesisContext::init_program_after(usize index) {
    return ::synthesis_init_program_net_after(*this, index);
}

DecomposeContext *SynthesisContext::decompose_ctx() const  {
    return ::synthesis_decompose_ctx(*this);
}

QVariant SynthesisContext::toVariant() const {
    QVariantHash result;
    result["c_matrix"] = c_matrix()->toVariant();

    QVariantList programsList;
    for (int i = 0; i < programs(); i++) {
        QVariantHash programHash;
        programHash["net_after"] = program_net_after(i)->toVariant();

        QVariantList data;
        for (int j = 0; j < program_size(i); j++) {
            data.push_back(program_value(i, j));
        }
        programHash["data"] = data;

        programsList << programHash;
    }

    result["programs"] = programsList;

    return result;
}

void SynthesisContext::fromVariant(const QVariant &data) {
    auto map = data.toHash();

    // Восстановим тензор преобразования
    c_matrix()->fromVariant(map["c_matrix"]);

    // Восстановим синтезированные программы
    auto programsList = map["programs"].toList();
    for (auto& program : programsList) {
        add_program();

        auto programHash = program.toHash();
        auto net = init_program_after(programs() - 1);
        net->fromVariant(programHash["net_after"]);

        auto programValues = programHash["data"].toList();
        for (int i = 0; i < programValues.size(); i++) {
            set_program_value(programs() - 1, i, programValues[i].toInt());
        }
    }
}

void SynthesisContext::drop() {
    ::synthesis_delete(this);
}

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

QVariant CMatrix::toVariant() const {
    QVariantHash result;
    result["rows"] = rows();
    result["columns"] = columns();

    QVariantList data;
    for (int i = 0; i < rows(); i++) {
        for (int j = 0; j < columns(); j++) {
            data << index(i, j);
        }
    }
    result["data"] = data;

    return result;
}

void CMatrix::fromVariant(const QVariant &data) {
    auto map = data.toHash();
    auto raw = map["data"].toList();
    for (int i = 0; i < rows(); i++) {
        for (int j = 0; j < columns(); j++) {
            set_value(i, j, raw[i * columns() + j].toInt());
        }
    }
}