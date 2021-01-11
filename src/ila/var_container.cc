/// \file
/// Source code for the class VarContainer and its derived classes.

#include <ilang/ila/var_container.h>
#include <ilang/ila/ast_hub.h>

namespace ilang {

VarContainerPtr VarContainer::Make(const std::string& name, const SortPtr& t) {
  std::string prefix = name + "_";  // also makes name a bit more private if last variable.
  switch (t->uid()) {
    case AstUidSort::kBool:
      return VarContainerPtr{new VarPrimitive{asthub::NewBoolVar(name)}};
    case AstUidSort::kBv:
      return VarContainerPtr{new VarPrimitive{asthub::NewBvVar(name, t->bit_width())}};
    case AstUidSort::kMem:
      return VarContainerPtr{new VarPrimitive{asthub::NewMemVar(name, t->addr_width(), t->data_width())}};
    case AstUidSort::kVec: 
      {
        vector_container vc {};
        for (int i = 0; i != t->vec_size(); ++i) {
          vc.push_back(Make(prefix + std::to_string(i) + "_", t->data_atom()));
        }
        return VarContainerPtr{new VarVector{t, vc}};
      }
    case AstUidSort::kStruct:
      {
        struct_container sc {};
        for (auto& [name, type] : t->members()) {
          sc.push_back({name, Make(prefix + name + "_", type)});
        }
        return VarContainerPtr{new VarStruct(t, sc)};
      }
    default:
      ILA_ASSERT(false) << "can't make VarContainer: recieved unknown type";
      return nullptr;
  }
}

VarContainerPtr VarContainer::from_primitive_expr(const ExprPtr& p) {
  return VarContainerPtr{new VarPrimitive{p}};
}

ExprPtr VarContainer::to_primitive_expr() {
  invalid_operation_error_("conversion to primitive expr");
  return {nullptr};
}

VarContainerPtr VarContainer::nth(size_t idx) {
  invalid_operation_error_("vector element access");
  return {nullptr};
}

size_t VarContainer::size() const {
  invalid_operation_error_("getting vector size");
  return 0;
}

const VarContainer::vector_container& VarContainer::elements() {
  invalid_operation_error_("getting vector elements");
  return empty_vec_;
}


VarContainerPtr VarContainer::member(const std::string& name) {
  invalid_operation_error_("struct member access");
  return {nullptr};
}

const VarContainer::struct_container& VarContainer::members() {
  invalid_operation_error_("getting struct members");
  return empty_struct_;
}

VarContainer::partition VarContainer::order_preserving_partition(
  size_t n_parts, std::function<size_t(size_t)> which_part
) {
  invalid_operation_error_("partitioning");
  return empty_vec_;
}

VarContainerPtr VarContainer::zip() {
  invalid_operation_error_("zipping members");
  return {nullptr};
}

VarContainerPtr VarContainer::unzip() {
  invalid_operation_error_("unzipping elements");
  return {nullptr};
}

 VarContainerPtr VarContainer::project(const std::vector<std::string>& names) {
   invalid_operation_error_("projection");
   return {nullptr};
 }

VarContainerPtr VarContainer::project_without(
    const std::vector<std::string>& names
) {
  invalid_operation_error_("projection");
  return {nullptr};
}

VarContainerPtr VarContainer::join_with(const VarContainerPtr& b) {
  invalid_operation_error_("struct joining");
  return {nullptr};
}


/* VarPrimitive */

VarPrimitive::VarPrimitive(ExprPtr var):  VarContainer(var->sort()), impl_ {var} {}

void VarPrimitive::visit_with(const VarContainer::visitor& visit) { visit(this); }

/* VarVector */

VarVector::VarVector(const SortPtr& t, const vector_container& elems): 
  VarContainer(t), impl_ {elems} {}

void VarVector::visit_with(const VarContainer::visitor& visit) {
  visit(this);
  for (auto& elem : impl_) {
    elem->visit_with(visit);
  }
}

VarContainerPtr VarVector::nth(size_t idx) { 
  ILA_ASSERT (idx < impl_.size()) << " array access out of bounds";
  return impl_[idx];
}

VarContainer::partition VarVector::order_preserving_partition(
    size_t n_parts, std::function<size_t(size_t)> which_part
) {
  std::vector<vector_container> parts(n_parts);
  for (int i = 0; i != size(); ++i) {
    size_t index = which_part(i);
    ILA_ASSERT(index < n_parts) << "partition function out of bounds";
    parts[index].push_back(impl_[i]);
  }
  std::vector<VarContainerPtr> result;
  for (int i = 0; i != n_parts; ++i) {
    auto s = Sort::MakeVectorSort(sort()->data_atom(), parts[i].size());
    result.emplace_back(new VarVector{s, parts[i]});
  }
  return result;
}

VarContainerPtr VarVector::unzip() {
  ILA_ASSERT (sort()->data_atom()->is_struct()) 
    << "can't unzip vector of data-atoms if they aren't structs";
  ILA_ASSERT (!sort()->data_atom()->members().empty()) 
    << "can't unzip vector of empty structs";
  std::vector<std::pair<std::string, SortPtr>> res_sort {};
  std::unordered_map<std::string, vector_container> res {};
  for (const auto& [name, da] : sort()->data_atom()->members()) {
    std::cout << name << ": " << Sort::MakeVectorSort(da, size()) << std::endl;
    res_sort.emplace_back(name, Sort::MakeVectorSort(da, size()));
    res[name].reserve(size());
  }
  for (auto& x : elements()) {
    for (auto& [name, m] : x->members()) {
      res[name].push_back(m);
    }
  }
  return from_cpp_obj(Sort::MakeStructSort(res_sort), res);
}

/* VarStruct */

VarStruct::VarStruct(const SortPtr& t, const struct_container& members): 
  VarContainer(t), impl_ {members} {}

void VarStruct::visit_with(const visitor& visit) {
  visit(this);
  for (auto& [_, elem] : impl_) {
    elem->visit_with(visit);
  }
}

VarContainerPtr VarStruct::member(const std::string& name) {
  for (auto& [n, elem] : impl_) {
    if (n == name) return elem;
  }
  ILA_ASSERT(false) << "member '" + name + "' not found";
  return nullptr;
}

VarContainerPtr VarStruct::zip() {
  ILA_ASSERT (!members().empty()) << "can't zip empty struct";
  int size = -1;
  std::vector<std::pair<std::string, SortPtr>> da {};
  for (const auto& [name, v] : sort()->members()) {
    ILA_ASSERT(v->is_vec()) << "expected vector";
    if (size < 0) size = v->vec_size();
    else { 
      ILA_ASSERT(size == v->vec_size())
        << "expected vector of size " << size
        << "got vector of size " << v->vec_size();
    }
    da.emplace_back(name, v->data_atom());
  }

  std::vector<std::vector<std::pair<std::string, VarContainerPtr>>> vec(size);
  for (const auto& [name, v] : members()) {
    for (int i = 0; i != size; ++i) {
      vec[i].emplace_back(name, v->nth(i));
    }
  }
  return from_cpp_obj(Sort::MakeVectorSort(Sort::MakeStructSort(da), size), vec);
}

 VarContainerPtr VarStruct::project(const std::vector<std::string>& names) {
  std::unordered_map<std::string, VarContainerPtr> ms {members().begin(), members().end()};
  std::vector<std::pair<std::string, SortPtr>> srt;
  struct_container projection {};
  for (const auto& name : names) {
    ILA_ASSERT(ms.find(name) != ms.end()) 
      << "member with name " << name << " not found";
    srt.emplace_back(name, ms[name]->sort());
    projection.emplace_back(name, ms[name]);
  }
  return from_cpp_obj(Sort::MakeStructSort(srt), projection);
 }

VarContainerPtr VarStruct::project_without(
    const std::vector<std::string>& names
) {
  std::unordered_map<std::string, VarContainerPtr> 
    ms {members().begin(), members().end()};
  for (const auto& name : names) ms.erase(name);

  std::vector<std::pair<std::string, SortPtr>> projsort {};
  for (const auto& [name, s] : sort()->members()) {
    if (ms.find(name) != ms.end()) projsort.emplace_back(name, s);
  }
  return from_cpp_obj(Sort::MakeStructSort(projsort), ms);
}

VarContainerPtr VarStruct::join_with(const VarContainerPtr& b) {
  ILA_ASSERT (b->is_struct()) << "can't join non-structs";
  std::unordered_map<std::string, VarContainerPtr> ms {members().begin(), members().end()};
  ms.insert(b->members().begin(), b->members().end());
  
  if (ms.size() != members().size() + b->members().size()) {
    ILA_ASSERT(false) << "structs being merged contained duplicate keys";
    return {nullptr};
  }

  auto joinsort {sort()->members()};
  auto bsort_members {b->sort()->members()};
  joinsort.insert(joinsort.end(), bsort_members.begin(), bsort_members.end());
  return from_cpp_obj(Sort::MakeStructSort(joinsort), ms);
}

}