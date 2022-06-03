# Queue_dlist
The queue_dist is a queue that implements the dlist library from this repository.
The only requirement for utilizing this library is to supply a comparison 
function. Since the data in each node is generic, it has no idea what the data
in the nodes are. The comparison function is necessary to be able to properly 
fetch items in the queue.
