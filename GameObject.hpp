#pragma once

// IDK HOW WELL THE ROTATION WORKS BUT I AM GOING TO MARK IT DONE

#include "Core.hpp"

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vex {
	class GameObject {
		public:
			std::string name;
			std::function<void(float)> Update = [](float dt){};
			std::function<void()> Render = [](){};
		protected:
			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 positionOffsetFromParent;
			glm::vec3 rotationOffsetFromParent;
			Vex::GameObject* parent;
			std::vector<GameObject*> children;

		public:
			GameObject(std::string name, glm::vec3 position, glm::vec3 rotation);
			~GameObject();

			auto MakeChild(Vex::GameObject* parent) -> void;

			auto GetPosition() -> glm::vec3;
			auto GetRotation() -> glm::vec3;

			auto SetPosition(glm::vec3 position) -> void;
			auto SetRotation(glm::vec3 rotation) -> void;
			auto SetRotation(glm::vec3 rotation, glm::vec3 origin) -> void;

			auto SetChildPosition(glm::vec3 position) -> void;

			auto Move(glm::vec3 adjustment) -> void;
			auto Rotate(glm::vec3 rotation) -> void;
			auto Rotate(glm::vec3 rotation, glm::vec3 origin) -> void;

			auto GenerateRotMatrix() -> glm::mat4;

			virtual auto internal_render() -> void = 0;
	};

	std::map<std::string, Vex::GameObject*> object_table;
}

Vex::GameObject::GameObject(std::string name, glm::vec3 position, glm::vec3 rotation) {
	if (object_table.find(name) != object_table.end()){
		std::cout << "Error at Vex::GameObject::GameObject(std::string, glm::vec3, glm::vec3) in GameObject.hpp\nGameObject \"" << name << "\" already exists" << std::endl;
		return;
	}
	object_table.insert(std::make_pair(name, this));
	this->name = std::move(name);
	this->position = position;
	this->rotation = rotation;
}

Vex::GameObject::~GameObject() {
	if (object_table.find(this->name)->second == this){
		object_table.erase(this->name);
	}
}

auto Vex::GameObject::MakeChild(Vex::GameObject* parent) -> void {
	this->parent = parent;
	this->positionOffsetFromParent = this->position - parent->GetPosition();
	this->rotationOffsetFromParent = this->rotation - parent->GetRotation();
}

auto Vex::GameObject::GetPosition() -> glm::vec3 {
	return this->position;
}

auto Vex::GameObject::GetRotation() -> glm::vec3 {
	return this->rotation;
}

auto Vex::GameObject::SetPosition(glm::vec3 position) -> void {
	this->position = position;
	for (auto& child : this->children){
		child->SetChildPosition(position);
	}
}

auto Vex::GameObject::SetRotation(glm::vec3 rotation) -> void {
	glm::vec3 changeInRotation = rotation - this->rotation;
	this->rotation = rotation;
	for (auto& child : this->children){
		child->Rotate(changeInRotation, this->position);
	}
}

auto Vex::GameObject::SetRotation(glm::vec3 rotation, glm::vec3 origin) -> void {
	glm::vec3 changeInRotation = rotation - this->rotation;
	this->Rotate(changeInRotation, origin);
	for (auto& child : this->children){
		child->Rotate(changeInRotation);
	}
}

auto Vex::GameObject::SetChildPosition(glm::vec3 position) -> void {
	this->position = position + this->positionOffsetFromParent;
	for (auto& child : this->children) {
		child->SetChildPosition(this->position);
	}
}

auto Vex::GameObject::Move(glm::vec3 adjustment) -> void {
	this->position += adjustment;
	for (auto& child : this->children) {
		child->Move(adjustment);
	}
}

auto Vex::GameObject::Rotate(glm::vec3 rotation) -> void {
	this->rotation += rotation;
	for (auto& child : this->children) {
		child->Rotate(rotation, this->position);
	}
}

auto Vex::GameObject::Rotate(glm::vec3 rotation, glm::vec3 origin) -> void {
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 pos_rot_h = rotateZ * rotateY * rotateX * glm::vec4(this->position - origin, 1.0f);
	this->position = glm::vec3(pos_rot_h) + origin;
	this->rotation = rotation + this->rotationOffsetFromParent;

	for (auto& child : this->children) {
		child->Rotate(rotation, origin);
	}
}

auto Vex::GameObject::GenerateRotMatrix() -> glm::mat4 {
	glm::mat4 rotMatrix = glm::mat4(1.0f);
	rotMatrix = glm::rotate(rotMatrix, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	rotMatrix = glm::rotate(rotMatrix, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	rotMatrix = glm::rotate(rotMatrix, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return rotMatrix;
}
