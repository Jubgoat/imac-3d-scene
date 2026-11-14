public class Task {
    private final String taskId;
    private String name;
    private String description;

    public Task(String taskId, String name, String description) {
        // Validate taskId
        if (taskId == null || taskId.length() > 10) {
            throw new IllegalArgumentException("Task ID must be non-null and cannot exceed 10 characters");
        }
        
        // Validate name
        if (name == null || name.length() > 20) {
            throw new IllegalArgumentException("Name must be non-null and cannot exceed 20 characters");
        }
        
        // Validate description
        if (description == null || description.length() > 50) {
            throw new IllegalArgumentException("Description must be non-null and cannot exceed 50 characters");
        }
        
        this.taskId = taskId;
        this.name = name;
        this.description = description;
    }

    // Getters
    public String getTaskId() {
        return taskId;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    // Setters (only for updatable fields)
    public void setName(String name) {
        if (name == null || name.length() > 20) {
            throw new IllegalArgumentException("Name must be non-null and cannot exceed 20 characters");
        }
        this.name = name;
    }

    public void setDescription(String description) {
        if (description == null || description.length() > 50) {
            throw new IllegalArgumentException("Description must be non-null and cannot exceed 50 characters");
        }
        this.description = description;
    }
}