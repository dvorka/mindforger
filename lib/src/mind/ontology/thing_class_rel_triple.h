/*
 thing_class_rel_triple.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef M8R_THING_CLASS_REL_TRIPLE_H_
#define M8R_THING_CLASS_REL_TRIPLE_H_

#include <string>
#include <set>

#include "../../config/color.h"

/*
 * Thing, Class, Relationship, RelationshipType and Triple
 * share the same header file to simplify compilation units
 * dependencies for the compiler.
 */

namespace m8r {

class Relationship;

/**
 * @brief Ontology Thing.
 *
 * See m8r::Ontology.
 */
class Thing
{
protected:
    /**
     * @brief Display name.
     */
    std::string name;

    /**
     * @brief Relationships.
     *
     * Explicit relationships (both incoming and outgoing) distinquished
     * using subject.
     */
    std::set<Relationship*> relationships;

public:
    Thing();
    explicit Thing(const std::string name);
    Thing(const Thing&) = delete;
    Thing(const Thing&&) = delete;
    Thing &operator=(const Thing&) = delete;
    Thing &operator=(const Thing&&) = delete;
    virtual ~Thing();

    const std::string& getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }

    const std::set<Relationship*> getRelationships() const { return relationships; }
    size_t getRelationshipsCount() const { return relationships.size(); }
};

/**
 * @brief Ontology class.
 *
 * See m8r::Ontology.
 */
class Clazz : public Thing
{
public:
    static constexpr const auto ROOT_CLASS = nullptr;

private:
    /**
     * @brief Parent class in ISA hierarchy.
     *
     * All Classes are organized to ISA (C++ is-a programming language) hierarchy.
     * The root of ISA hierarchy is the Thing (instance of m8r::Class) defined by Ontology.
     */
    Clazz* isA;

public:
    explicit Clazz(const std::string& name, Clazz* isA);
    Clazz(const Clazz&) = delete;
    Clazz(const Clazz&&) = delete;
    Clazz &operator=(const Clazz&) = delete;
    Clazz &operator=(const Clazz&&) = delete;
    ~Clazz();

    Clazz* getIsA() const { return isA; }
    void setIsA(Clazz* isA) { this->isA = isA; }
};

/**
 * @brief Ontology reationship type.
 *
 * See m8r::Ontology.
 */
class RelationshipType : public Clazz
{
private:
    bool reflexive; // Thing op Thing (==,>=,*1) tj. *1(T) = T, ==(T) = T, ...
    bool symetric;
    bool transitive;

    const Color& color;

public:
    // static initialization order fiasco prevention
    static const std::string& KeyIsA() {
        static const std::string KEY_IS_A= std::string{"is a"};
        return KEY_IS_A;
    }
    static const std::string& KeySameAs() {
        static const std::string KEY_SAME_AS = std::string{"same as"};
        return KEY_SAME_AS;
    }
    static const std::string& KeyOppositeOf() {
        static const std::string KEY_OPPOSITE_OF= std::string{"opposite of"};
        return KEY_OPPOSITE_OF;
    }
    static const std::string& KeyDependsOn() {
        static const std::string KEY_DEPENDS_ON= std::string{"depends on"};
        return KEY_DEPENDS_ON;
    }

public:
    RelationshipType() = delete;
    explicit RelationshipType(const std::string& name, Clazz* isA, const Color& color);
    RelationshipType(const RelationshipType&) = delete;
    RelationshipType(const RelationshipType&&) = delete;
    RelationshipType &operator=(const RelationshipType&) = delete;
    RelationshipType &operator=(const RelationshipType&&) = delete;
    ~RelationshipType();

    bool isReflexive() const { return reflexive; }
    void setReflexive(bool r) { reflexive = r; }
    bool isTransitive() const { return transitive; }
    void setTransitive(bool t) { transitive = t; }
    bool isSymetric() const { return symetric; }
    void setSymetric(bool s) { symetric = s; }
};

/**
 * @brief Ontology triple.
 *
 * Subject -predicate-> Object
 *
 * See m8r::Ontology.
 */
class Triple : public Thing
{
protected:
    const Thing* subject;
    const RelationshipType* predicate;
    const Thing* object;

public:
    explicit Triple(const std::string name, const Thing* subject, const RelationshipType* predicate, const Thing* object);
    Triple(const Triple&) = delete;
    Triple(const Triple&&) = delete;
    Triple &operator=(const Triple&) = delete;
    Triple &operator=(const Triple&&) = delete;
    ~Triple();

    const Thing* getSubject() const { return subject; }
    void setSubject(const Thing* subject) { this->subject = subject; }
    const RelationshipType* getPredicate() const { return predicate; }
    void setPredicate(const RelationshipType* predicate) { this->predicate = predicate; }
    const Thing* getObject() const { return object; }
    void setObject(const Thing* object) { this->object = object; }
};

/**
 * @brief Ontology reationship.
 *
 * See m8r::Ontology.
 */
// IMPROVE consider Relationship and Triple to be template
class Relationship : public Triple
{
public:
    /**
     * @brief Triple constructor - every Relationship is a Triple.
     */
    explicit Relationship(const std::string name, Thing* subject, RelationshipType* predicate, Thing* object);
    Relationship(const Relationship&) = delete;
    Relationship(const Relationship&&) = delete;
    Relationship &operator=(const Relationship&) = delete;
    Relationship &operator=(const Relationship&&) = delete;
    ~Relationship();
};

} // m8r namespace

#endif /* M8R_THING_CLASS_REL_TRIPLE_H_ */
