#include "blob_tracker.hpp"
#include <algorithm>

using namespace std;

blob_tracker::blob_tracker(const int &new_maxdist) : maxdist(new_maxdist), nextid(1)
{

}

blob_tracker::~blob_tracker()
{

}

void blob_tracker::update(const blobs_t& blobs)
{
	map<int, blob> ntracked;
	for (auto &b : blobs)
	{
		auto it = min_element(tracked.begin(), tracked.end(), [&b](const pair<int, blob> &lhs, const pair<int, blob> &rhs)
		{
			return cv::norm(lhs.second.center - b.center) < cv::norm(rhs.second.center - b.center);
		});

		if (it != tracked.end() && cv::norm(it->second.center - b.center) < maxdist)
		{
			ntracked.insert(make_pair(it->first, b));
		}
		else
		{
			ntracked.insert(make_pair(nextid++, b));
		}
	}
	tracked = move(ntracked);
}

void blob_tracker::clear()
{
	tracked.clear();
}

const std::map<int, blob>& blob_tracker::get_all() const
{
	return tracked;
}

std::map<int, blob>& blob_tracker::get_all()
{
	return tracked;
}

boost::optional<blob> blob_tracker::operator[] (const int& id)
{
	auto it = tracked.find(id);
	if (it != tracked.end())
		return it->second;
	else
		return boost::none;
}

int blob_tracker::best()
{
	auto it = min_element(tracked.begin(), tracked.end(), [](const pair<int, blob> &lhs, const pair<int, blob> &rhs)
	{
		return lhs.second.score < rhs.second.score;
	});

	if (it != tracked.end())
		return it->first;
	else
		return 0;
}
