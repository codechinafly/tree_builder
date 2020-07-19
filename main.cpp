#include <assert.h>
#include <time.h>
#include <thread>
#include "tree_builder.hpp"

struct catalog_item
{
	std::string sipid;
	std::string parentID;
	std::string businessID;
	std::string civilCode;
	std::string name;
	bool used;
};

class my_tree_node : public tree_builder<my_tree_node>::tree_node
{
public:
	bool has_parent() const override
	{
		return (!item.parentID.empty() || !item.businessID.empty() || !item.civilCode.empty());
	}

	bool is_parent(const std::shared_ptr<my_tree_node>& child_node) const override
	{
		if (child_node->item.parentID.size() > 0)
		{
			if (item.sipid == child_node->item.parentID) return true;
		}
		else if (child_node->item.businessID.size() > 0)
		{
			if (item.sipid == child_node->item.businessID) return true;
		}
		else if (child_node->item.civilCode.size() > 0)
		{
			if (item.sipid == child_node->item.civilCode) return true;
		}
		return false;
	}

	bool is_equal(const std::shared_ptr<my_tree_node>& node) const override
	{
		return (node->item.sipid == item.sipid);
	}

	std::string display_name() const override
	{
		return item.name + "(" + item.sipid + ")";
	}

	catalog_item item;
};

int main()
{
	constexpr std::size_t catalog_count = 24;
	catalog_item cc[catalog_count] =
	{
		{"3402", "", "", "", "²âÊÔ»ù²ãµ¥Î»ĞĞÕşÇø»®", false},
		{"34020000001310000001", "34020000002160000001", "", "3402", "²âÊÔÉãÏñ»ú1", false},
		{"34020000001310000002", "34020000002160000001", "", "3402", "²âÊÔÉãÏñ»ú2", false},
		{"34020000001310000003", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú3", false},
		{"34020000001310000004", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú4", false},
		{"34020000001310000005", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú5", false},
		{"34020000001310000006", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú6", false},
		{"34020000001310000007", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú7", false},
		{"34020000001310000008", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú8", false},
		{"34020000001310000009", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú9", false},
		{"34020000001310000010", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú10", false},
		{"34020000001310000011", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú11", false},
		{"34020000001310000012", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú12", false},
		{"34020000001310000013", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú13", false},
		{"34020000001310000014", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú14", false},
		{"34020000001310000015", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú15", false},
		{"34020000001310000016", "34020000002000000001", "", "3402", "²âÊÔÉãÏñ»ú16", false},
		{"34020000001340000001", "34020000001310000001", "", "3402", "²âÊÔ±¨¾¯Æ÷1", false},
		{"34020000001340000002", "34020000001310000002", "", "3402", "²âÊÔ±¨¾¯Æ÷2", false},
		{"34020000001370000001", "34020000001310000001", "", "3402", "²âÊÔÒôÆµÊä³öÉè±¸1", false},
		{"34020000001370000002", "34020000001310000002", "", "3402", "²âÊÔÒôÆµÊä³öÉè±¸2", false},
		{"34020000002000000001", "", "", "3402", "µ÷²âÆ½Ì¨", false},
		{"34020000002150000001", "34020000002000000001", "", "3402", "²âÊÔÒµÎñ·Ö×é", false},
		{"34020000002160000001", "", "34020000002150000001", "3402", "²âÊÔĞéÄâ×éÖ¯", false},
	};

	catalog_item root_item = { "root", "", "", "", "¸ù½Úµã", true};
	auto root_node = std::make_shared<my_tree_node>();
	root_node->item = root_item;
	tree_builder<my_tree_node > tb(root_node);

	std::size_t count = 0;
	srand((unsigned int)::time(0));
	while (true)
	{
		if (count == catalog_count) break;
		int r = rand() % catalog_count;
		if (cc[r].used)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		cc[r].used = true;
		auto node = std::make_shared<my_tree_node>();
		node->item = cc[r];
		tb.insert_node(node);
		++count;
	}

	tb.final_check();

	auto total_count = tb.total_count();
	assert(total_count == catalog_count);
	printf("total count: %zu\n", total_count);

	{ // test find
		int r = rand() % catalog_count;
		auto dst = std::make_shared<my_tree_node>();
		dst->item = cc[r];
		auto fnode = tb.find_node(root_node, dst);
		assert(fnode);
		printf("found: dst=%s found=%s\n", dst->item.sipid.c_str(), fnode->item.sipid.c_str());

		dst = std::make_shared<my_tree_node>();
		dst->item.sipid = "N/A";
		fnode = tb.find_node(root_node, dst);
		assert(!fnode);
		printf("not found: dN=%s fN=%p\n", dst->item.sipid.c_str(), fnode.get());
	}

	tb.dump_tree_to_file("tree.txt");

	printf("tree builder complete\n");
}
