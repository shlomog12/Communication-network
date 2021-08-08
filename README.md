# Final_communication_project

## General Explanation to the Examiner:
We implemented all the functions such that they will work on graphs both with cycles and without cycles.
How we did it:
We created a set if visited nodes which is sent in the Discover message, the set saves all the nodes which we already visited, that's how every node 'knows' to avoid neighbors that were
already visited.
In addition, every node that gets a Route message about a certain route saves the route, and every time a nodes connects or disconnects its neighbors broadcast to all the the nodes in 
this connected component, so they 'know' they need to delete all the routes and compute them again, beacuse the graph has changed.
