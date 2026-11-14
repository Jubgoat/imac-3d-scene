import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.BeforeEach;
import static org.junit.jupiter.api.Assertions.*;

public class TaskTest {
    private Task task;

    @BeforeEach
    void setUp() {
        task = new Task("1234567890", "Test Task", "This is a test description");
    }

    @Test
    void testTaskCreation() {
        assertNotNull(task);
        assertEquals("1234567890", task.getTaskId());
        assertEquals("Test Task", task.getName());
        assertEquals("This is a test description", task.getDescription());
    }

    @Test
    void testTaskIdNotNull() {
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            new Task(null, "Valid Name", "Valid Description");
        });
        assertTrue(exception.getMessage().contains("Task ID must be non-null"));
    }

    @Test
    void testTaskIdMaxLength() {
        // Test with 11 characters (should fail)
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            new Task("12345678901", "Valid Name", "Valid Description");
        });
        assertTrue(exception.getMessage().contains("cannot exceed 10 characters"));

        // Test with exactly 10 characters (should pass)
        assertDoesNotThrow(() -> {
            new Task("1234567890", "Valid Name", "Valid Description");
        });
    }

    @Test
    void testNameNotNull() {
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            new Task("123", null, "Valid Description");
        });
        assertTrue(exception.getMessage().contains("Name must be non-null"));
    }

    @Test
    void testNameMaxLength() {
        // Test with 21 characters (should fail)
        String longName = "A".repeat(21);
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            new Task("123", longName, "Valid Description");
        });
        assertTrue(exception.getMessage().contains("cannot exceed 20 characters"));

        // Test with exactly 20 characters (should pass)
        String exactName = "A".repeat(20);
        assertDoesNotThrow(() -> {
            new Task("123", exactName, "Valid Description");
        });
    }

    @Test
    void testDescriptionNotNull() {
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            new Task("123", "Valid Name", null);
        });
        assertTrue(exception.getMessage().contains("Description must be non-null"));
    }

    @Test
    void testDescriptionMaxLength() {
        // Test with 51 characters (should fail)
        String longDesc = "A".repeat(51);
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            new Task("123", "Valid Name", longDesc);
        });
        assertTrue(exception.getMessage().contains("cannot exceed 50 characters"));

        // Test with exactly 50 characters (should pass)
        String exactDesc = "A".repeat(50);
        assertDoesNotThrow(() -> {
            new Task("123", "Valid Name", exactDesc);
        });
    }

    @Test
    void testSetName() {
        task.setName("Updated Name");
        assertEquals("Updated Name", task.getName());
    }

    @Test
    void testSetNameValidation() {
        String longName = "A".repeat(21);
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            task.setName(longName);
        });
        assertTrue(exception.getMessage().contains("cannot exceed 20 characters"));
    }

    @Test
    void testSetDescription() {
        task.setDescription("Updated Description");
        assertEquals("Updated Description", task.getDescription());
    }

    @Test
    void testSetDescriptionValidation() {
        String longDesc = "A".repeat(51);
        Exception exception = assertThrows(IllegalArgumentException.class, () -> {
            task.setDescription(longDesc);
        });
        assertTrue(exception.getMessage().contains("cannot exceed 50 characters"));
    }
}