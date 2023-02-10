//
// Created by 15857 on 2023/2/8.
//
#include "stdio.h"
#include "git2.h"
#include "vector.h"
#include "commit.h"
#include "blob.h"
#include "tag.h"
#include "tree.h"
#include "str.h"
static git_odb *_odb;
static git_repository *_repo;

struct exhaustive_state {
	git_odb *db;
	git_vector commits;
};

/** Get all commits from the repository. */
static int exhaustive_objs(const git_oid *id, void *payload)
{
	struct exhaustive_state *mc = (struct exhaustive_state *)payload;
	size_t header_len;
	git_object_t header_type;
    long size;
    const unsigned char *data;
    git_odb_object *obj;
	int error = 0;
	error = git_odb_read_header(&header_len, &header_type, mc->db, id);
	if (header_type == GIT_OBJECT_COMMIT) {
        puts("commit");
        if(git_odb_read(&obj, mc->db, id) < 0)
            goto end;
        data = (const unsigned char *)git_odb_object_data(obj);
        size = (long)git_odb_object_size(obj);
        error = git_rocks_odb_write(id, mc->db, data, size, GIT_OBJECT_COMMIT);
        git_object_free(obj);
	}
    else if(header_type == GIT_OBJECT_TREE) {
        puts("tree");
        git_tree *tree = NULL;
        git_tree_lookup(&tree, _repo, id);
        git_treebuilder *tb = NULL;
        git_treebuilder_new(&tb, _repo, tree);
        git_treebuilder_write_cache(id, tb);
        error = git_rocks_odb_write(id, mc->db, tb->write_cache.ptr, tb->write_cache.size , GIT_OBJECT_TREE);
        git_treebuilder_free(tb);
    }
    else if(header_type == GIT_OBJECT_BLOB){
        puts("blob");
        git_blob *blob = NULL;
        if(git_blob_lookup(&blob, _repo, id) < 0)
            goto end;
        size = (long)git_blob_rawsize(blob);
        const void * blob_rawcontent = git_blob_rawcontent(blob);
        error = git_rocks_odb_write(id, mc->db, blob_rawcontent, size, GIT_OBJECT_BLOB);
        git_blob_free(blob);
    }
    else if(header_type == GIT_OBJECT_TAG){
        return 0;
        puts("tag");
        if(git_odb_read(&obj, mc->db, id) < 0)
            goto end;
        data = (const unsigned char *)git_odb_object_data(obj);
        size = git_odb_object_size(obj);
        error = git_rocks_odb_write(id, mc->db, data, size, GIT_OBJECT_COMMIT);
        git_object_free(obj);
    }

    end:
    if(error < 0) {
        char shortsha[41] = {0};
        git_oid_tostr(shortsha, 40, id);
        printf("error id %s\n", shortsha);
        printf("error raw =\n");
        p_write(fileno(stdout), data, (size_t)size);
        puts("");
    }
	return 0;
}

/** 根据tree需要单独遍历保存 */

/** Get all reference from the repository. */
static int exhaustive_refs(const git_oid *id, void *payload) {
    return 0;
}

static int foreach_cb(const git_oid *oid, void *data)
{
	int *nobj = data;
	(*nobj)++;

	// GIT_UNUSED(oid);
    printf("%d %s\n", *nobj, git_oid_tostr_s(oid));

	return 0;
}

int main(){
    git_libgit2_init();

	int nobj = 0;
    int error = 0;
    const char *path = "../tmp_2023";
	error = git_repository_open(&_repo, path);
    if(error) {
        puts("repo error");
        return 0;
    }
	error = git_repository_odb(&_odb, _repo);
    if(error) {
        puts("odb error");
        return 0;
    }
    // git_odb_foreach(_odb, foreach_cb, &nobj);
    // printf("%d\n", nobj);

    /*
     * 遍历获取Objects信息
     * 放到rocksdb存储库中
     * */
    struct exhaustive_state mc = {
            .db = NULL,
            .commits = GIT_VECTOR_INIT,
    };

    git_repository_odb(&mc.db, _repo);
    git_odb_foreach(mc.db, &exhaustive_objs, &mc);

    git_libgit2_shutdown();
    return 0;
}
