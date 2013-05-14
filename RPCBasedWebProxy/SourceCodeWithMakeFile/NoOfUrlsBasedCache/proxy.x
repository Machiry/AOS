struct webpageResponse {
    string webPageContents<3000>;
};
typedef struct webpageResponse webpageResponse;

program PROXY_FETCH_PROG {
        version PROXY_FETCH_VERS {
                webpageResponse PROXY_FETCH(string) = 1;
        } = 1;
} = 0x23451111;
