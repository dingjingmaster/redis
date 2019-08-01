cmake_minimum_required (VERSION 3.0)        # 最低版本要求

project (redis)                             # 项目名

INCLUDE_DIRECTORIES("src" "deps/hiredis")
INCLUDE_DIRECTORIES("src" "deps/lua/src/")

#aux_source_directory(./src/ REDIS_SRC)
#aux_source_directory(./deps/* HIREDIS_SRC)

SET(REDIS_SRC
        ./src/adlist.c;
        ./src/ae.c;
        ./src/anet.c;
        ./src/dict.c;
        ./src/redis.c;
        ./src/sds.c;
        ./src/zmalloc.c;
        ./src/lzf_c.c;
        ./src/lzf_d.c;
        ./src/pqsort.c;
        ./src/zipmap.c;
        ./src/sha1.c;
        ./src/ziplist.c;
        ./src/release.c;
        ./src/networking.c;
        ./src/util.c;
        ./src/object.c;
        ./src/db.c;
        ./src/replication.c;
        ./src/rdb.c;
        ./src/t_string.c;
        ./src/t_list.c;
        ./src/t_set.c;
        ./src/t_zset.c;
        ./src/t_hash.c;
        ./src/config.c;
        ./src/aof.c;
        ./src/pubsub.c;
        ./src/multi.c;
        ./src/debug.c;
        ./src/sort.c;
        ./src/intset.c;
        ./src/syncio.c;
        ./src/cluster.c;
        ./src/crc16.c;
        ./src/endianconv.c;
        ./src/slowlog.c;
        ./src/scripting.c;
        ./src/bio.c;
        ./src/rio.c;
        ./src/rand.c;
        ./src/memtest.c;
        ./src/crc64.c;
        ./src/bitops.c;
        ./src/sentinel.c;
        ./src/notify.c;
        ./src/setproctitle.c;
        ./src/blocked.c;
        ./src/hyperloglog.c;
)

# redis-server
#add_subdirectory(deps/hiredis/)
link_directories("./deps/lua/src" "./deps/hiredis/" "deps/jemalloc/lib")
link_libraries(m)
link_libraries(lua)
link_libraries(hiredis)
link_libraries(pthread)
link_libraries(jemalloc)
add_executable(redis_server ${REDIS_SRC} ${HIREDIS_SRC})



