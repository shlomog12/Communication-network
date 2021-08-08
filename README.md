# Final Project - Computer Network

##### The above project implement connection between different nodes in an unidirectional and unweighted graph.

Each node contains its own ID number, socket and in addition the ability to listen to 100 different nodes from each other.

Among other things, it is possible to send and receive messages from different nodes that are not necessarily adjacent to each other using different types of messages that allow that.

Each message has an ID number, a source node, a destination node, the number of nodes to deliver the message if its necessary the type of the message and 492 bytes that contain the information depending on the type of message.



*The functions in the program are divided into two types, the first is functions called from the user and the second type are functions that are activated by the nodes themselves.*

this type of messages :



<a href="https://ibb.co/yyQC4S7"><img src="https://i.ibb.co/mqcKN8g/ddd.png" alt="ddd" border="0"></a>


<a href="https://ibb.co/6mRL2jJ"><img src="https://i.ibb.co/W6WjQmp/image-20210808225812259.png" alt="image-20210808225812259" border="0"></a>

We implemented all the functions such that they will work on graphs both with cycles and without cycles.
How we did it:
We created a set if visited nodes which is sent in the Discover message, the set saves all the nodes which we already visited, that's how every node 'knows' to avoid neighbors that were
already visited.
In addition, every node that gets a Route message about a certain route saves the route, and every time a nodes connects or disconnects its neighbors broadcast to all the the nodes in 
this connected component, so they 'know' they need to delete all the routes and compute them again, because the graph has changed.

