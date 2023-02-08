# Get new commits

通过libgit2仅获取项目更新的内容。

目前可以使用`git_odb_write`insert一个test object了。

TODO:
- [x] 遍历repo的object，全部insert到rocksdb中
- [ ] 基于rocksdb验证能否实现增量更新
- [ ] 跑通git_odb_write_multi_pack_index,实现从pack文件读取object数据
- [ ] git_odb_open_wstream,据说比git_odb_write快


# locate commit
定位commit最先出现的分支。