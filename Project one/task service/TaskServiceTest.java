import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.BeforeEach;
import static org.junit.jupiter.api.Assertions.*;

public class TaskServiceTest {
    private TaskService taskService;
    private Task task1;
    private Task task2;

    @BeforeEach
    void setUp() {
        taskService = new TaskService();
        task1 = new Task("1", "Task One", "First task description");
        task2 = new Task("2", "Task Two", "Second task description");
    }

    @Test
    void testAddTask() {
        taskService.addTask(task1);
        assertTrue(taskService.taskExists("1"));
    }

    @Test
    void testAddDuplicateTask() {
        taskService.addTask(task1);
        
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            taskService.addTask(task1);
        });
        assertTrue(exception.getMessage().contains("Task ID already exists"));
    }

    @Test
    void testAddNullTask() {
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            taskService.addTask(null);
        });
        assertTrue(exception.getMessage().contains("Task cannot be null"));
    }

    @Test
    void testDeleteTask() {
        taskService.addTask(task1);
        assertTrue(taskService.taskExists("1"));
        
        taskService.deleteTask("1");
        assertFalse(taskService.taskExists("1"));
    }

    @Test
    void testDeleteNonExistentTask() {
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            taskService.deleteTask("999");
        });
        assertTrue(exception.getMessage().contains("Task ID not found"));
    }

    @Test
    void testUpdateTaskName() {
        taskService.addTask(task1);
        taskService.updateTaskName("1", "Updated Task Name");
        
        Task updatedTask = taskService.getTask("1");
        assertEquals("Updated Task Name", updatedTask.getName());
    }

    @Test
    void testUpdateTaskNameNonExistentTask() {
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            taskService.updateTaskName("999", "New Name");
        });
        assertTrue(exception.getMessage().contains("Task ID not found"));
    }

    @Test
    void testUpdateTaskDescription() {
        taskService.addTask(task1);
        taskService.updateTaskDescription("1", "Updated description text");
        
        Task updatedTask = taskService.getTask("1");
        assertEquals("Updated description text", updatedTask.getDescription());
    }

    @Test
    void testUpdateTaskDescriptionNonExistentTask() {
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            taskService.updateTaskDescription("999", "New Description");
        });
        assertTrue(exception.getMessage().contains("Task ID not found"));
    }

    @Test
    void testMultipleTasks() {
        taskService.addTask(task1);
        taskService.addTask(task2);
        
        assertTrue(taskService.taskExists("1"));
        assertTrue(taskService.taskExists("2"));
        
        taskService.deleteTask("1");
        assertFalse(taskService.taskExists("1"));
        assertTrue(taskService.taskExists("2"));
    }

    @Test
    void testUpdateNameValidationThroughService() {
        taskService.addTask(task1);
        
        String longName = "A".repeat(21);
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            taskService.updateTaskName("1", longName);
        });
        assertTrue(exception.getMessage().contains("cannot exceed 20 characters"));
    }

    @Test
    void testUpdateDescriptionValidationThroughService() {
        taskService.addTask(task1);
        
        String longDesc = "A".repeat(51);
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            taskService.updateTaskDescription("1", longDesc);
        });
        assertTrue(exception.getMessage().contains("cannot exceed 50 characters"));
    }
}