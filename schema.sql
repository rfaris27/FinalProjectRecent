USE ATM;

CREATE TABLE IF NOT EXISTS accounts (
    AccountNumber INT AUTO_INCREMENT PRIMARY KEY,
    Login         VARCHAR(50)    UNIQUE NOT NULL,
    Pin           VARCHAR(5)     NOT NULL,
    HolderName    VARCHAR(100)   NOT NULL,
    Balance       DECIMAL(10,2)  DEFAULT 0.00,
    Status        VARCHAR(20)    DEFAULT 'Active',
    IsAdmin       BOOLEAN        DEFAULT FALSE
) AUTO_INCREMENT = 100;

-- Admin (assignment example) — rows exist in MySQL only AFTER you load this file (see README).
INSERT INTO accounts (Login, Pin, HolderName, Balance, Status, IsAdmin)
VALUES ('Adnan123', '12345', 'Adnan', 160000.00, 'Active', TRUE)
ON DUPLICATE KEY UPDATE Login = Login;

-- Sample customer
INSERT INTO accounts (Login, Pin, HolderName, Balance, Status, IsAdmin)
VALUES ('John456', '11111', 'John Doe', 5000.00, 'Active', FALSE)
ON DUPLICATE KEY UPDATE Login = Login;
