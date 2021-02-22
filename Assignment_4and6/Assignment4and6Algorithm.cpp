

main{
    open the feeds file
    initialize database
    initialize thread package
    set up three semaphores : PreviouslySeenArticlesOpen, indicesOpen, stopWordsOpen as members of the database
    Welcome {
       read and print the contents of the welcome file
    }
    LoadStopWords {
        read the stopWords from file,and store them in the database's stopWords hashset
    }
    BuildIndices {
        Initialize database's indices and previouslySeenArticles
        initialize semaphore rssFeedAcess(4) to manage number of rssFeeds
        for each rss feed listed in the file {
        make a thread to go through rssFeed as below
            processFeed {
                if a connection to the feed can be made {
                    semaphoteWait(rssFeedAccess)
                    PullAllNewsItems {
                        initialize Semaphore urlconnOpen(24)  to limit number of open urls to 24
                        for each url in the feed {
                            processStartTag {
                                define a new rssFeedEntry and add title and url
                            }
                            ProcessEndTag {
                                make a thread to parse the articleand pull the indices as bellow{
                                    parseArticle {
                                        make a connnection to the news article

                                        semaphoreWait(PreviouslySeenArticlesOpen)
                                        add to the previouslySeenArticles, 
                                        semaphoreSignal(PreviouslySeenArticlesOpen)
                                        
                                        semaphoreWait(urlOpen)
                                        ScanArticle{
                                            for each word in the article {
                                                AddWordToIndices{
                                                    if the wordIsWortIndexing, add it to indices,
                                                    useing indicesOpensemaphore
                                                }
                                            }
                                        }
                                        semaphoreSignal(urlOpen)
                                    }        
                                }        
                            }       
                        }
                    }
                    semaphoteSignal(rssFeedAccess)
                }        
            }
        }
    }
    run all threads
    QueryIndices {
        allow user to sepecify a single search term
    }
    processResoponse {
        list 10 relevant articles
    }
    SemaphoreFree {
        free all the semaphores
    }
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
