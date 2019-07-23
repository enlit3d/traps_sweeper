#pragma once

#include <vector>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <cmath>

namespace engine::collections
{
	template <typename T>
	class SegmentsList {
		struct Segment {
			T start;
			T end;
			Segment(T start, T end) : start(start), end(end){}
			friend bool operator < (const Segment& a, const Segment& b) {
				return a.end < b.start;
			}
			friend bool operator == (const Segment& a, const Segment& b) {
				return !(a < b) && !(b < a);
			}
		};
	public:
		void insert(T min, T max) {
			// larger so that border hits like [0, 10] [10, 20] are included and the neighbour segments merged
			auto searchSeg = expandedSegment(min, max);
			auto range = std::equal_range(begin(), end(), searchSeg);
			if (range.second - range.first > 0) { // found overlaps
				// expand values
				min = std::min(min, range.first->start);
				max = std::max(max, (range.second - 1)->end);
				// remove the rest
				if (range.second - range.first > 1) { // is this if needed? not sure...
					_segs.erase(range.first + 1, range.second);
				}
				// modify the first segment found (more efficient than delete then insert)
				auto modified = range.first;
				modified->start = min;
				modified->end = max;
			}
			else { // no overlaps, find pos and insert
				auto it = std::lower_bound(begin(), end(), Segment{ min, max });
				_segs.emplace(it, min, max);
			}
			//assertSelf();
		}

		void remove(T min, T max) {
			auto range = std::equal_range(begin(), end(), Segment{ min, max });
			auto first = range.first;
			auto last = range.second;
			if (last - first == 0) { 
				// complete miss!
				return; 
			}
			auto elemsHit = last - first;
			if (elemsHit == 1) { // need to handle the only hit 1 segment case separately because may need to insert new segments
				Segment& seg = *first;
				Segment left = Segment{ seg.start, std::min(seg.end, min) };
				Segment right = Segment{ std::max(seg.start, max), seg.end };
				if (left.end > left.start) {
					if (right.end > right.start) {
						// keeps both segments, need to insert a new segment
						seg = left;
						_segs.insert(first + 1, right);
					}
					else {
						// keep left segment only
						seg = left;
					}
				}
				else {
					// keep right segment only
					seg = right;
				}
				//assertSelf();
			}
			else { 
				Segment& smallest = *first;
				Segment& largest = *(last-1);

				auto removeStart = first + 1;
				auto removeEnd = last - 1;
				// clip the end segments, and delete them if they became invalid
				smallest.end = std::min(smallest.end, min);
				if (smallest.end <= smallest.start) {
					removeStart = first;
				}
				largest.start = std::max(largest.start, max);
				if (largest.end <= largest.start) {
					removeEnd = last;
				}
				if (removeEnd > removeStart) { 
					_segs.erase(removeStart, removeEnd); 
				}
				//assertSelf();
			}
			//assertSelf();
		}

		bool contains(T min, T max) const {
			auto searchSeg = Segment(min, max);
			auto it = std::lower_bound(begin(), end(), searchSeg);
			return it != end() && *it == searchSeg;
		}

		auto begin() { return _segs.begin(); }
		auto end() { return _segs.end(); }
		const auto begin() const { return _segs.begin(); }
		const auto end() const { return _segs.end(); }
	private:
		std::vector<Segment> _segs = {};
		void assertSelf() {
			for (auto& seg : _segs) {
				if (seg.end < seg.start) { throw; }
			}
		}
	private:
		Segment expandedSegment(T min, T max) const {
			Segment searchSeg{ min, max };
			if constexpr (std::is_integral_v<T>) {
				searchSeg = { min - 1, max + 1 };
			}
			else {
				searchSeg = { std::nextafter(min, std::numeric_limits<T>::min()), std::nextafter(max, std::numeric_limits<T>::max()) };
			}
			return searchSeg;
		}
	};
}