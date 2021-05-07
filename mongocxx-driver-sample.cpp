#include <cstdint>
#include <iostream>
#include <vector>
#include <utility>
#include <chrono>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;

int main() {
    //连接池
    mongocxx::instance instance{};
    mongocxx::uri uri("mongodb://localhost:27017/?minPoolSize=3&maxPoolSize=3");
    mongocxx::pool pool{uri};

    /* 单连接
    mongocxx::client client(uri);

    mongocxx::database db = client["mydb"];
    mongocxx::collection coll = db["test"];
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc = builder
    << "name" << "MongoDB"
    << bsoncxx::builder::stream::finalize;
    coll.insert_one(std::move(doc));
    */

   // 插入简单文档
    mongocxx::pool::entry client = pool.acquire();
    auto coll = (*client)["mydb"]["test"];
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc = builder
    << "name" << "simple"
    << bsoncxx::builder::stream::finalize;

    //可能抛出异常  
    coll.insert_one(std::move(doc));

    // 插入嵌套文档
    bsoncxx::document::value doc2 = builder
    << "name" << "complex"
    << "type" << "database"
    << "count" << 1
    << "versions" << bsoncxx::builder::stream::open_array
    << "v3.2" << "v3.0" << "v2.6"
    << close_array
    << "info" << bsoncxx::builder::stream::open_document
    << "x" << 203
    << "y" << 102
    << bsoncxx::builder::stream::close_document
    << "date" << bsoncxx::types::b_date(std::chrono::system_clock::now())
    << bsoncxx::builder::stream::finalize;

    coll.insert_one(std::move(doc2));

    // 获取文档内容
    //bug
    //bsoncxx::document::element element = doc2.view()["name"];


    // 分页查询
    mongocxx::options::find opts;
    opts.limit(100).skip(10);
    mongocxx::cursor cursor = coll.find({}, opts);
    for(auto doc : cursor) {
        std::cout<<bsoncxx::to_json(doc).c_str()<<std::endl;
    }
}