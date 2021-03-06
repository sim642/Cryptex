#ifndef BLOB_TRACKER_H
#define BLOB_TRACKER_H

#include "blob.hpp"
#include <map>
#include <boost/optional.hpp>

class ball_targeter; // forward declare to avoid circular dependency

class blob_tracker
{
	public:
		blob_tracker(const int &new_maxdist);
		virtual ~blob_tracker();

		void update(const blobs_t &blobs);
		void clear();

		const std::map<int, blob>& get_all() const;
	private:
		friend ball_targeter;
		std::map<int, blob>& get_all();

	public:
		boost::optional<blob> operator[] (const int &id);
		int best();

	private:
		const int maxdist;

		int nextid;
		std::map<int, blob> tracked;
};

#endif // BLOB_TRACKER_H
