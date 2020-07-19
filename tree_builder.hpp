#pragma once
#include <list>
#include <string>
#include <memory>
#include <fstream>

template<class node_type>
class tree_builder
{
public:
	using node_ptr = std::shared_ptr<node_type>;

	using child_node_list = std::list<node_ptr>;

	class tree_node
	{
	public:
		friend class tree_builder;

		virtual bool has_parent() const = 0;

		virtual bool is_parent(const node_ptr& child_node) const = 0;

		virtual bool is_equal(const node_ptr& node) const = 0;

		virtual std::string display_name() const = 0;

		const child_node_list& child_list() { return child_list_; }

	private:
		child_node_list child_list_;
	};

	tree_builder(const node_ptr& root_node)
	{
		total_count_ = 0;
		root_node_ = root_node;
	}

	const node_ptr& root_node() const
	{
		return root_node_;
	}

	void insert_node(const node_ptr& node)
	{
		if (node->has_parent())
		{
			const auto parent_node = __find_parent_node(node, root_node_);
			if (parent_node && parent_node != node && parent_node != root_node_)
				parent_node->child_list_.emplace_back(node);
			else root_node_->child_list_.emplace_back(node);
		}
		else root_node_->child_list_.push_back(node);
		++total_count_;
	}

	void insert_to_parent_node(const node_ptr& node, const node_ptr& parent_node)
	{
		parent_node->child_list_.emplace_back(node);
		++total_count_;
	}

	void final_check()
	{
		typename child_node_list::iterator it_end = root_node_->child_list_.end();
		for (auto it = root_node_->child_list_.begin(); it != it_end;)
		{
			const node_ptr& node = (*it);
			if (node->has_parent() && !root_node_->is_parent(node))
			{
				const auto parent_node = __find_parent_node(node, root_node_);
				if (parent_node && parent_node != node && parent_node != root_node_)
				{
					parent_node->child_list_.emplace_back(node);
					it = root_node_->child_list_.erase(it);
				}
				else ++it;
			}
			else ++it;
		}
	}

	const node_ptr find_node(const node_ptr& src, const node_ptr& dst)
	{
		for (const auto& it : src->child_list_)
		{
			if (dst->is_equal(it)) return it;
			else
			{
				auto node = find_node(it, dst);
				if (node) return node;
			}
		}
		return node_ptr();
	}

	std::size_t total_count()
	{
		return total_count_;
	}

	void clear()
	{
		total_count_ = 0;
		root_node_->child_list_.clear();
	}

	void dump_tree_to_file(std::string path)
	{
		std::ofstream ofs(path, std::ios::binary);
		if (!ofs.is_open()) return;
		__dump_node_to_file(root_node_, 0, ofs);
		ofs.close();
	}

private:
	const node_ptr __find_parent_node(const node_ptr& child_node, const node_ptr& parent_node)
	{
		if (parent_node->is_parent(child_node))
		{
			return parent_node;
		}
		else
		{
			for (const auto& it : parent_node->child_list_)
			{
				auto node = __find_parent_node(child_node, it);
				if (node) return node;
			}
		}
		return node_ptr();
	}

	void __dump_node_to_file(const node_ptr& node, int space_i, std::ofstream& ofs)
	{
		ofs << "©À";
		for (int i = 0; i < space_i; ++i)
			ofs << "©¤©¤";
		ofs << node->display_name() << "\r\n";
		for (const auto& it : node->child_list_)
			__dump_node_to_file(it, space_i + 1, ofs);
	}

private:
	node_ptr root_node_;
	std::size_t total_count_;
};
