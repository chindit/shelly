#include "Request.h"

bool Request::call(const std::string &url, const std::vector<std::pair<std::string, std::string>> &headers, const std::string &payload, std::string &resultBody) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return false;
    }
    CURLcode res;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                     Request::write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultBody);

    // Adding headers
    struct curl_slist *curlHeaders = nullptr;

    for (auto &it : headers)
    {
        curlHeaders = curl_slist_append(curlHeaders, (it.first + ":" + it.second).c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
    // Uncomment for easy debugging
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    bool isFailed = res != CURLE_OK;
    if(isFailed) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_slist_free_all(curlHeaders);

    return !isFailed;
}

size_t Request::write_data(void* ptr, size_t size, size_t nmemb, void* buffer)
{
    ((std::string*) buffer)->append((char*) ptr, nmemb);
    return nmemb;
}
