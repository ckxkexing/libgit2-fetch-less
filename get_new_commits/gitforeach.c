//
// Created by 15857 on 2023/2/8.
//
#include "stdio.h"
#include "git2.h"
#include "vector.h"
#include "commit.h"
#include "blob.h"
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
	int error = 0;

	error = git_odb_read_header(&header_len, &header_type, mc->db, id);
	if (error < 0)
		return error;

	if (header_type == GIT_OBJECT_COMMIT) {
		git_commit *commit = NULL;
		git_commit_lookup(&commit, _repo, id);
        if(commit == NULL) return 0;
        // printf("%s\n", commit->raw_message);
        // printf("%s\n", commit->raw_header);
        // puts("");
        git_rocks_odb_write(id, mc->db, commit, strlen(commit), GIT_OBJECT_COMMIT);
		// git_vector_insert(&mc->commits, commit);
	}
    else if(header_type == GIT_OBJECT_TREE) {
        git_tree *tree = NULL;
        git_tree_lookup(&tree, _repo, id);
        if(tree == NULL ) return 0;
        int error = git_rocks_odb_write(id, mc->db, tree, strlen(tree), GIT_OBJECT_TREE);
        if(error < 0) {
            int t = 2 / 0;
        }
    }
    else if(header_type == GIT_OBJECT_BLOB){
        git_blob *blob = NULL;
        git_blob_lookup(&blob, _repo, id);
        if(blob == NULL ) return 0;
        // printf("%s\n", blob->data);
        int error = git_rocks_odb_write(id, mc->db, blob, strlen(blob), GIT_OBJECT_BLOB);
        if(error < 0) {
            int t = 2 / 0;
        }
    }
    else if(header_type == GIT_OBJECT_TAG){
        git_tag *tag = NULL;
        git_tag_lookup(&tag , _repo, id);
        if(tag == NULL ) return 0;
        int error = git_rocks_odb_write(id, mc->db, tag, strlen(tag), GIT_OBJECT_TAG);
        if(error < 0) {
            int t = 2 / 0;
        }
    }

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
    const char *path = "../cargo-c";
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
