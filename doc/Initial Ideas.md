## Initial Ideas

- A repository can contain more than one project.

- Located in each project root is a file named `.Manifest`, that lists the paths to all files in the project, as well as their version and hash.

- This project treats version control as an inherently centralized system:

  > A version control system consists of some remote server that holds the repository and manages changes to it, and any number of user clients that can fetch projects from the repository…

- The main advantage the professor cites for VC is backup.

- The job of the server: handle multiple (simultaneous!) connections, compare files to determine similarity

