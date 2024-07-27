/* vim:set ts=2 sw=2 sts=2 et: */
/**
 * \author     Marcus Holland-Moritz (github@mhxnet.de)
 * \copyright  Copyright (c) Marcus Holland-Moritz
 *
 * This file is part of dwarfs.
 *
 * dwarfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dwarfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dwarfs.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <ranges>

#include <dwarfs/error.h>
#include <dwarfs/internal/global_entry_data.h>
#include <dwarfs/options.h>

namespace dwarfs::internal {

template <typename T, typename U>
std::vector<T> global_entry_data::get_vector(map_type<T, U> const& map) const {
  std::vector<std::pair<T, U>> pairs{map.begin(), map.end()};
  std::ranges::sort(pairs, [](auto const& p1, auto const& p2) {
    return p1.second < p2.second;
  });
  auto view = std::views::keys(pairs);
  return std::vector<T>{view.begin(), view.end()};
}

auto global_entry_data::get_uids() const -> std::vector<uid_type> {
  return options_.uid ? std::vector<uid_type>{*options_.uid}
                      : get_vector(uids_);
}

auto global_entry_data::get_gids() const -> std::vector<gid_type> {
  return options_.gid ? std::vector<gid_type>{*options_.gid}
                      : get_vector(gids_);
}

auto global_entry_data::get_modes() const -> std::vector<mode_type> {
  return get_vector(modes_);
}

auto global_entry_data::get_names() const -> std::vector<std::string> {
  return get_vector(names_);
}

auto global_entry_data::get_symlinks() const -> std::vector<std::string> {
  return get_vector(symlinks_);
}

void global_entry_data::index(map_type<std::string, uint32_t>& map) {
  auto keys = std::views::all(map) | std::views::keys;
  std::vector<std::string_view> tmp{keys.begin(), keys.end()};
  std::ranges::sort(tmp);
  std::decay_t<decltype(map)>::mapped_type ix{0};
  for (auto& s : tmp) {
    map[s] = ix++;
  }
}

uint64_t global_entry_data::get_time_offset(uint64_t time) const {
  return (time - timestamp_base_) / options_.time_resolution_sec;
}

uint64_t global_entry_data::get_mtime_offset(uint64_t time) const {
  return !options_.timestamp ? get_time_offset(time) : UINT64_C(0);
}

uint64_t global_entry_data::get_atime_offset(uint64_t time) const {
  return !options_.timestamp && options_.keep_all_times ? get_time_offset(time)
                                                        : UINT64_C(0);
}

uint64_t global_entry_data::get_ctime_offset(uint64_t time) const {
  return !options_.timestamp && options_.keep_all_times ? get_time_offset(time)
                                                        : UINT64_C(0);
}

uint64_t global_entry_data::get_timestamp_base() const {
  return (options_.timestamp ? *options_.timestamp : timestamp_base_) /
         options_.time_resolution_sec;
}

size_t global_entry_data::get_uid_index(uid_type uid) const {
  return options_.uid ? 0 : DWARFS_NOTHROW(uids_.at(uid));
}

size_t global_entry_data::get_gid_index(gid_type gid) const {
  return options_.gid ? 0 : DWARFS_NOTHROW(gids_.at(gid));
}

size_t global_entry_data::get_mode_index(mode_type mode) const {
  return DWARFS_NOTHROW(modes_.at(mode));
}

uint32_t global_entry_data::get_name_index(std::string const& name) const {
  return DWARFS_NOTHROW(names_.at(name));
}

uint32_t
global_entry_data::get_symlink_table_entry(std::string const& link) const {
  return DWARFS_NOTHROW(symlinks_.at(link));
}

void global_entry_data::add_uid(uid_type uid) {
  if (!options_.uid) {
    add(uid, uids_, next_uid_index_);
  }
}

void global_entry_data::add_gid(gid_type gid) {
  if (!options_.gid) {
    add(gid, gids_, next_gid_index_);
  }
}

void global_entry_data::add_mtime(uint64_t time) {
  if (time < timestamp_base_) {
    timestamp_base_ = time;
  }
}

void global_entry_data::add_atime(uint64_t time) {
  if (options_.keep_all_times) {
    add_mtime(time);
  }
}

void global_entry_data::add_ctime(uint64_t time) {
  if (options_.keep_all_times) {
    add_mtime(time);
  }
}

} // namespace dwarfs::internal
